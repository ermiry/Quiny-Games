#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quiny/quiny.h"

#include "mongo/mongo.h"

#include "types/myString.h"

#include "cerver/cerver.h"
#include "cerver/client.h"
#include "cerver/http/parser.h"

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

// TODO: maybe add a header, the data is used just as raw data, and we can create a method to actually create the response???
// TODO: maybe we can have a similar model for requests in the framework??
typedef struct HttpResponse {

    unsigned int status;
    char *data;
    size_t data_len;

} HttpResponse;

HttpResponse *http_response_new (void) {

    HttpResponse *res = (HttpResponse *) malloc (sizeof (HttpResponse));
    if (res) {
        memset (res, 0, sizeof (HttpResponse));
        res->data = NULL;
    } 

    return res;

}

HttpResponse *http_response_create (unsigned int status, const char *data, size_t data_len) {

    HttpResponse *res = NULL;
    if (data) {
        res = (HttpResponse *) malloc (sizeof (HttpResponse));
        if (res) {
            res->status = status;
            memcpy (res->data, data, data_len);
            res->data_len = data_len;
        }
    }

    return res;

}

void http_respponse_delete (HttpResponse *res) {

    if (res) {
        if (res->data) free (res->data);
        free (res);
    }

}

int http_response_send_to_socket (const HttpResponse *res, const int socket_fd) {

    int retval = 1;

    if (res) retval = send (socket_fd, res->data, res->data_len, 0);

    return retval;

}

typedef enum ServerType {

    FILE_SERVER = 1,
    WEB_SERVER, 
    GAME_SERVER

} ServerType;

typedef enum ValueType {

    VALUE_INT,
    VALUE_DOUBLE,
    VALUE_STRING,

} ValueType;

// TODO: maybe create a json utily function with bson??
typedef struct JsonKeyValue {

    String *key;
    void *value;
    ValueType valueType;

} JsonKeyValue;

// uses the reference to a value, do not free the value, it will be free when the list gets destroy
JsonKeyValue *json_key_value_new (void) {

    JsonKeyValue *jkvp = (JsonKeyValue *) malloc (sizeof (JsonKeyValue));
    if (jkvp) {
        jkvp->key = NULL;
        jkvp->value = NULL;
    }

    return jkvp;

}

JsonKeyValue *json_key_value_create (const char *key, const void *value, ValueType value_type) {

    JsonKeyValue *jkvp = NULL;

    if (key && value) {
        jkvp = (JsonKeyValue *) malloc (sizeof (JsonKeyValue));
        if (jkvp) {
            jkvp->key = str_new (key);
            jkvp->value = value;
            jkvp->valueType = value_type;
        }
    }

    return jkvp;

}

void json_key_value_delete (void *ptr) {

    if (ptr) {
        JsonKeyValue *jkvp = (JsonKeyValue *) ptr;

        if (jkvp->key) str_delete (jkvp->key);
        if (jkvp->valueType == VALUE_STRING) str_delete ((String *) jkvp->key);

        free (jkvp);
    }

}


static char *game_ask_handler (DoubleList *pairs) {

    if (pairs) {
        // get the action to perform
        const char *action = NULL;
        KeyValuePair *kvp = NULL;
        for (ListElement *le = dlist_start (pairs); le; le = le->next) {
            kvp = (KeyValuePair *) le->data;
            if (!strcmp (kvp->key, "action")) {
                action = kvp->value;
                break;
            }
        }

        if (action) {
            if (!strcmp (action, "test")) {

            }
        }

        else logMsg (stdout, ERROR, NO_TYPE, "No action provided for ask game!");
    }

}

// TODO: handle the creation of generic responses with a struct and a function!!
// FIXME: how can we handle global errors?
static void quiny_main_handler (RecvdBufferData *data, DoubleList *pairs) {

    if (pairs) {
        HttpResponse *res = NULL;

        // first search for what game we need
        const char *game = NULL;
        KeyValuePair *kvp = NULL;
        for (ListElement *le = dlist_start (pairs); le; le = le->next) {
            kvp = (KeyValuePair *) le->data;
            if (!strcmp (kvp->key, "game")) {
                game = kvp->value;
                break;
            }
        }

        // if a game was found, send it to the game action handler
        if (game) {
            if (!strcmp (game, "ask")) res = game_ask_handler (pairs);

            else logMsg (stdout, WARNING, NO_TYPE, createString ("Got unknown game %s", game));
        }

        // if no game was found, search for another action
        else {
            const char *action = NULL;
            for (ListElement *le = dlist_start (pairs); le; le = le->next) {
                kvp = (KeyValuePair *) le->data;
                if (!strcmp (kvp->key, "action")) {
                    action = kvp->value;
                    break;
                }
            }

            // handle the action
            if (!strcmp (action, "test")) {
                // FIXME: send json instead
                // char res[1024] = "HTTP/1.1 200 OK\r\n\nHello World";
                // send (data->sock_fd, res, strlen (res), 0);
            }

            else logMsg (stdout, WARNING, NO_TYPE, createString ("Got unkown action %s", action));
        }

        // send the response to the client
        http_response_send_to_socket (res, data->sock_fd);

        // after the performed action, close the client socket
        client_disconnect_by_socket (data->server, data->sock_fd);

        dlist_destroy (pairs);
    }

}

/**** 
// TODO: maybe handle the creation of a new thread to handle this from the server_recieve
****/

// FIXME: getting sigsev if we get a requets withput query!!
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
                char str[50];
                snprintf (str, 50, "%.*s", (int) path_len, path);
                printf ("%s\n", str);
                char *query = http_strip_path_from_query (str);
                printf ("%s\n", query);
                
                int count = 0;
                const char *first = query;
                const char *last = first + strlen (query);
                DoubleList *pairs = http_parse_query_into_pairs (query, last);

                // now we can handle the action and its values
                quiny_main_handler (data, pairs);

                // KeyValuePair *kvp = NULL;
                // for (ListElement *le = dlist_start (pairs); le; le = le->next) {
                //     kvp = (KeyValuePair *) le->data;
                //     printf ("key: %s - value: %s\n", kvp->key, kvp->value);
                // }
            }
        }

        rcvd_buffer_data_delete (data);
    }

}

#pragma endregion