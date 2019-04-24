#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mongo/mongo.h"

#include "types/myString.h"

#include "cerver/cerver.h"
#include "cerver/client.h"

#include "cerver/game/game.h"
#include "cerver/game/player.h"
#include "cerver/game/lobby.h"

#include "cerver/http/parser.h"
#include "cerver/http/json.h"
#include "cerver/http/response.h"

#include "quiny/quiny.h"
#include "quiny/games/ask.h"

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

static void user_delete (void *ptr) {

    if (ptr) {
        User *user = (User *) ptr;

        str_delete (user->email);
        str_delete (user->name);
        str_delete (user->password);
        str_delete (user->username);

        free (user);
    }

}

// get a user doc from the db by oid
static const bson_t *user_find_by_oid (const bson_oid_t *oid) {

    if (oid) {
        bson_t *user_query = bson_new ();
        bson_append_oid (user_query, "_id", -1, oid);

        return mongo_find_one (users_collection, user_query);
    }

    return NULL;

}

// get a user doc from the db by username
static const bson_t *user_find_by_username (const char *username) {

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

// turn a user model into a json string
static char *quiny_user_json_create (const User *user, size_t *json_len) {

    char *retval = NULL;

    if (user && json_len) {
        bson_t *doc = bson_new ();
        if (doc) {
            bson_append_oid (doc, "_id", -1, &user->oid);
            bson_append_utf8 (doc, "name", -1, user->name->str, user->name->len);
            bson_append_utf8 (doc, "username", -1, user->username->str, user->username->len);
            bson_append_utf8 (doc, "email", -1, user->email->str, user->email->len);

            // TODO: do we need to free the doc after this?
            retval = bson_as_json (doc, json_len);
        }
    }

    return retval;

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

    // init activities
    if (ask_init ()) {
        logMsg (stderr, ERROR, GAME, "Failed to init ask game!");
        errors = 1;
    } 

    return errors;  

}

// clean up quiny data
int quiny_end (void) {

    int errors = 0;

    // close our collections handles
    if (users_collection) mongoc_collection_destroy (users_collection);

    // end activities
    if (ask_end) {
        logMsg (stderr, ERROR, GAME, "Failed to end ask game!");
        errors = 1;
    }

    mongo_disconnect ();

    return errors;

}

static const char *quiny_get_username (DoubleList *pairs) {

    const char *username = NULL;

    if (pairs) {
        KeyValuePair *kvp = NULL;
        for (ListElement *le = dlist_start (pairs); le; le = le->next) {
            kvp = (KeyValuePair *) le->data;
            if (!strcmp (kvp->key, "username")) {
                username = kvp->value;
                break;
            }
        }
    }

    return username;

}

static const char *quiny_get_password (DoubleList *pairs) {

    const char *username = NULL;

    if (pairs) {
        KeyValuePair *kvp = NULL;
        for (ListElement *le = dlist_start (pairs); le; le = le->next) {
            kvp = (KeyValuePair *) le->data;
            if (!strcmp (kvp->key, "password")) {
                username = kvp->value;
                break;
            }
        }
    }

    return username;

}

// FIXME: how do we sent back the token??
static HttpResponse *quiny_user_login (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        const char *username = quiny_get_username (pairs);
        const char *password = quiny_get_password (pairs);

        // search the user data from the db
        int errors;
        User *user = quiny_user_get (username, password, &errors);
        if (user) {
            // add the user as a new player in the game server
            // FIXME: generate my ids!!
            Player *player = player_new (NULL, "hola", user);
            player_set_delete_player_data (player, user_delete);
            player_register_to_server (server, player);

            // return the user data as json
            size_t json_len;
            char *user_json = quiny_user_json_create (user, &json_len);
            res = http_response_create (200, NULL, 0, user_json, json_len);
            user_delete (user);
            free (user_json);        // we copy the data into the response
        }

        else res = http_response_json_error ("Login failed!");
    }

    return res;

}

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
            if (!strcmp (game, "ask")) res = game_ask_handler (data->server, pairs);

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
                String *test = str_new ("Quiny works!");
                JsonKeyValue *jkvp = json_key_value_create ("msg", test, VALUE_TYPE_STRING);
                size_t json_len;
                char *json = json_create_with_one_pair (jkvp, &json_len);
                json_key_value_delete (jkvp);
                res = http_response_create (200, NULL, 0, json, json_len);
                free (json);        // we copy the data into the response
            }

            else if (!strcmp (action, "login")) res = quiny_user_login (data->server, pairs);

            else logMsg (stdout, WARNING, NO_TYPE, createString ("Got unkown action %s", action));
        }

        if (res) {
            // send the response to the client
            http_response_compile (res);
            printf ("Response: %s\n", res->res);
            http_response_send_to_socket (res, data->sock_fd);
            http_respponse_delete (res);
        }

        // after the performed action, close the client socket
        client_disconnect_by_socket (data->server, data->sock_fd);

        dlist_destroy (pairs);
    }

}

/**** 
// TODO: maybe handle the creation of a new thread to handle this from the server_recieve
****/

// FIXME: also parse path!!
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