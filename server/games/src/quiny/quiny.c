#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quiny/quiny.h"

#include "mongo/mongo.h"

#include "types/myString.h"

#include "cerver/cerver.h"

#include "utils/myUtils.h"
#include "utils/log.h"

const char *uri_string = "mongodb://localhost:27017";
const char *db_name = "quiny";

#pragma region Users

#define USERS_COLL_NAME         "users"
static mongoc_collection_t *users_collection = NULL;

static User *user_new (void) {

    User *user = (User *) malloc (sizeof (User));
    if (user) {
        memset (user, 0, sizeof (User));
        user->email = user->name = user->password = user->username = NULL;
    }

    return user;

}

static void user_delete (User *user) {

    if (user) {
        str_delete (user->email);
        str_delete (user->name);
        str_delete (user->password);
        str_delete (user->username);

        free (user);
    }

}

// get a user doc from the db by oid
const bson_t *user_find_by_oid (const bson_oid_t *oid) {

    if (oid) {
        bson_t *user_query = bson_new ();
        bson_append_oid (user_query, "_id", -1, oid);

        return mongo_find_one (users_collection, user_query);
    }

    return NULL;

}

// get a user doc from the db by username
const bson_t *user_find_by_username (const char *username) {

    if (username) {
        bson_t *user_query = bson_new ();
        bson_append_utf8 (user_query, "username", -1, username, -1);

        return mongo_find_one (users_collection, user_query);
    }

    return NULL;    

}

// parses a bson doc into a user model
static User *user_doc_parse (const bson_t *user_doc) {

    User *user = NULL;

    if (user_doc) {
        user = user_new ();

        bson_iter_t iter;
        bson_type_t type;

        if (bson_iter_init (&iter, user_doc)) {
            while (bson_iter_next (&iter)) {
                const char *key = bson_iter_key (&iter);
                const bson_value_t *value = bson_iter_value (&iter);

                if (!strcmp (key, "_id")) {
                    bson_oid_copy (&value->value.v_oid, &user->oid);
                    // const bson_oid_t *oid = bson_iter_oid (&iter);
                    // memcpy (&user->oid, oid, sizeof (bson_oid_t));
                }

                else if (!strcmp (key, "name") && value->value.v_utf8.str) 
                    user->name = str_new (value->value.v_utf8.str);

                else if (!strcmp (key, "email") && value->value.v_utf8.str) 
                    user->email = str_new (value->value.v_utf8.str);

                else if (!strcmp (key, "username") && value->value.v_utf8.str) 
                    user->username = str_new (value->value.v_utf8.str);

                else if (!strcmp (key, "password") && value->value.v_utf8.str) 
                    user->password = str_new (value->value.v_utf8.str);

                else logMsg (stdout, WARNING, NO_TYPE, createString ("Got unknown key %s when parsing user doc.", key));
            }
        }
    }

    return user;

}

static User *user_get (const char *username) {

    User *user = NULL;

    const bson_t *user_doc = user_find_by_username (username);
    if (user_doc) {
        user = user_doc_parse (user_doc);
        bson_destroy ((bson_t *) user_doc);
    }

    return user;

}

// search for a user with the given username
// if we find one, check if the password match
User *quiny_user_get (const char *username, const char *password, int *errors) {

    User *user = NULL;

    if (username && password) {
        user = user_get (username);
        if (user) {
            // check that the password is correct
            if (!strcmp (password, user->password->str)) *errors = NO_ERRORS;
            else {
                // password is incorrect
                *errors = WRONG_PASSWORD;
                user_delete (user);
                user = NULL;
            }
        }

        else {
            #ifdef QUINY_DEBUG
                logMsg (stderr, ERROR, DEBUG_MSG, 
                    createString ("Not user find with username: %s", username));
            #endif
            *errors = NOT_USER_FOUND;
        }
    }

    else *errors = SERVER_ERROR;

    return user;

}


#pragma endregion

#pragma region Public

#include "http/picoParser.h"

// init quiny data & processes
int quiny_init (void) {

    int errors = 0;

    // TODO: do we need to pass the username and the db?
    if (!mongo_connect ()) {
        // open handle to user collection
        users_collection = mongoc_client_get_collection (mongo_client, db_name, USERS_COLL_NAME);
        if (!users_collection) {
            logMsg (stderr, ERROR, NO_TYPE, "Failed to get handle to users collection!");
            errors = 1;
        }
    }

    else {
        logMsg (stderr, ERROR, NO_TYPE, "Failed to connect to mongo");
        errors = 1;
    }

    return errors;  

}

// clean up quiny data
int quiny_end (void) {

    // close our collections handles
    if (users_collection) mongoc_collection_destroy (users_collection);

    mongo_disconnect ();

}

// custom function to handle the received buffer from the server
void quiny_handle_recieved_buffer (void *rcvd_buffer_data) {

    // logMsg (stdout, DEBUG_MSG, NO_TYPE, "Quiny buffer handler.");

    if (rcvd_buffer_data) {
        RecvdBufferData *data = (RecvdBufferData *) rcvd_buffer_data;

        if (data->buffer && (data->total_size > 0)) {
            char *method, *path;
            int pret, minor_version;
            struct phr_header headers[100];
            size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
            ssize_t rret;

            // printf ("buffer: %s\n", data->buffer);

            prevbuflen = buflen;
            buflen += rret;
            /* parse the request */
            num_headers = sizeof (headers) / sizeof (headers[0]);
            pret = phr_parse_request (data->buffer, data->total_size, (const char **) &method, &method_len, (const char **) &path, &path_len,
                                    &minor_version, headers, &num_headers, prevbuflen);
            if (pret > 0) {
                // printf("\nrequest is %d bytes long\n", pret);
                // printf("method is %.*s\n", (int)method_len, method);
                // TODO: copy this path, then we need to parse it and finally we handle the actions with the data!!
                printf("path is %.*s\n", (int) path_len, path);
                // printf("HTTP version is 1.%d\n", minor_version);
                // printf("headers:\n");
                // for (int i = 0; i != num_headers; ++i) {
                //     printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
                //         (int)headers[i].value_len, headers[i].value);
                // }
                // printf ("\n");
            }
        }

        rcvd_buffer_data_delete (data);
    }

}

#pragma endregion