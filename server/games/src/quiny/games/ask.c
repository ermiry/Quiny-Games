#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quiny/quiny.h"

#include "mongo/mongo.h"

#include "types/myString.h"

#include "cerver/cerver.h"
#include "cerver/client.h"
#include "cerver/http/parser.h"
#include "cerver/http/json.h"
#include "cerver/http/response.h"

#include "utils/myUtils.h"
#include "utils/log.h"

static HttpResponse *game_ask_create_lobby (DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (pairs) {
        KeyValuePair *kvp = NULL;
        const char *username = NULL;
        for (ListElement *le = dlist_start (pairs); le; le = le->next) {
            kvp = (KeyValuePair *) le->data;
            if (!strcmp (kvp->key, "username")) {
                username = kvp->value;
                break;
            }
        }

        if (username) {
            // get the player info


            // create a new lobby
            // send back the new lobby data -> looby id
        }

        else {
            logMsg (stderr, ERROR, NO_TYPE, "No username provided to create a lobby!");
            res = http_response_json_error ("No username provided!");
        }
    }

    return res;

}

HttpResponse *game_ask_handler (DoubleList *pairs) {

    HttpResponse *res = NULL;

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
                String *test = str_new ("Ask game works!");
                JsonKeyValue *jkvp = json_key_value_create ("msg", test, VALUE_TYPE_STRING);
                size_t json_len;
                char *json = json_create_with_one_pair (jkvp, &json_len);
                json_key_value_delete (jkvp);
                res = http_response_create (200, NULL, 0, json, json_len);
                free (json);        // we copy the data into the response
            }

            else if (!strcmp (action, "create_lobby")) res = game_ask_create_lobby (pairs);

            else if (!strcmp (action, "join_lobby")) {
                // get the player info
                // get the lobby info
                // join the lobby
                // send back the lobby data
            }

            else if (!strcmp (action, "leave_lobby")) {
                // get the player info
                // get the lobby info
                // leave the lobby
                // send back a success message
            }
        }

        else logMsg (stdout, ERROR, NO_TYPE, "No action provided for ask game!");
    }

    return res;

}