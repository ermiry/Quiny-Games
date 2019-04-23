#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mongo/mongo.h"

#include "types/myString.h"

#include "cerver/cerver.h"
#include "cerver/client.h"
#include "cerver/http/parser.h"
#include "cerver/http/json.h"
#include "cerver/http/response.h"
#include "cerver/game/game.h"
#include "cerver/game/player.h"
#include "cerver/game/lobby.h"
#include "cerver/game/score.h"

#include "quiny/quiny.h"
#include "quiny/games/ask.h"

#include "utils/myUtils.h"
#include "utils/log.h"

static AskGameData *ask_game_data_new (ScoreBoard *sb, AskTopic topic) {

    AskGameData *data = (AskGameData *) malloc (sizeof (AskGameData));
    if (data) {
        memset (data, 0, sizeof (AskGameData));
        data->sb = sb;
        data->topic = topic;

        // FIXME: be sure of this!!!
        // we don't have ot clear the user data here!!!
        // we just have a reference to the real structure in the avl tree
        data->competitors = dlist_init (NULL, NULL);
        data->current_question = NULL;
    } 

    return data;

}

static void ask_game_data_delete (void *ptr) {

    if (ptr) {
        AskGameData *data = (AskGameData *) ptr;
        game_score_delete (data->sb);
        free (data);
    }

}

static AskTopic game_ask_get_topic (DoubleList *pairs) {

    AskTopic retval;

    const char *topic = http_query_pairs_get_value (pairs, "topic");
    if (!topic) retval = TOPIC_GENERAL;
    else {
        if (!strcmp (topic, "science")) retval = TOPIC_SCIENCE;
        else if (!strcmp (topic, "geography")) retval = TOPIC_GEOGRAPHY;
        else if (!strcmp (topic, "history")) retval = TOPIC_HISTORY;

        else retval = TOPIC_GENERAL;
    }

    return retval;

}

static HttpResponse *game_ask_create_lobby (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        const char *token = http_query_pairs_get_value (pairs, "sessionID");
        if (token) {
            // get the player info suing the token
            GameServerData *game_data = (GameServerData *) server->serverData;

            // get the player with the session id (token) from the request
            Player *player_query = player_new (NULL, token, NULL);
            Player *owner = player_get (game_data->players->root, player_comparator_by_session_id, player_query);
            player_delete (player_query);

            if (owner) {
                // create a new lobby
                Lobby *lobby = lobby_create (server, owner, 0);

                // we need to get the game settings
                // TODO:

                // set the game data
                // FIXME: create the scoreboard
                ScoreBoard *sb = NULL;
                AskTopic topic = game_ask_get_topic (pairs);
                AskGameData *game_data = ask_game_data_new (sb, topic);
                lobby_set_game_data (lobby, game_data, ask_game_data_delete);

                // we dont need a handler, so we set the lobby handler to NULL
                lobby_set_handler (lobby, NULL);

                // send back the new lobby data -> looby id
                // TODO:
            }

            else {
                logMsg (stderr, ERROR, NO_TYPE, "No user found!");
                res = http_response_json_error ("No user found!");
            }
        }

        else {
            logMsg (stderr, ERROR, NO_TYPE, "No user token provided to create a lobby!");
            res = http_response_json_error ("No user token provided!");
        }
    }

    return res;

}

static HttpResponse *game_ask_join_lobby (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        // FIXME:
        // const char *username = game_ask_get_username (pairs);
        // if (username) {
        //     // get the player info
        //     // get the lobby info
        //     // join the lobby
        //     // send back the lobby data
        // }

        // else {
        //     logMsg (stderr, ERROR, NO_TYPE, "No username provided to join a lobby!");
        //     res = http_response_json_error ("No username provided!");
        // }
    }

    return res;

}

static HttpResponse *game_ask_leave_lobby (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        // FIXME:
        // const char *username = game_ask_get_username (pairs);
        // if (username) {
        //     // get the player info
        //     // get the lobby info
        //     // leave the lobby
        //     // send back a success message
        // }

        // else {
        //     logMsg (stderr, ERROR, NO_TYPE, "No username provided to join a lobby!");
        //     res = http_response_json_error ("No username provided!");
        // }
    }

    return res;

}

/*** Alexa ***/

// FIXME:
static HttpResponse *game_ask_start (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        GameServerData *game_data = (GameServerData *) server->serverData;

        // we get a user token and we need to get back the lobby id the user is in
        // and start a new game

        // set the current question to a new one
        // get the active user
        // send back this info as a json
    }

    return res;

}

static HttpResponse *game_ask_question (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        GameServerData *game_data = (GameServerData *) server->serverData;

        // get the lobby token
        const char *lobby_token = http_query_pairs_get_value (pairs, "token");
        if (lobby_token) {
            logMsg (stdout, DEBUG_MSG, NO_TYPE, createString ("Got lobby id: %s", lobby_token));

            // get the lobby associated with the lobby id
            Lobby query = { .id = createString ("%s", lobby_token) };
            Lobby *lobby = lobby_get (game_data, &query);
            if (lobby) {
                // get the next user
                // get the next question
                // send back the question and data in a json
            }

            else {
                logMsg (stderr, ERROR, GAME, createString ("Failed to get lobby with id: %s", lobby_token));
                http_response_json_error ("Failed ot get lobby!");
            }
        }

        else {
            logMsg (stderr, ERROR, GAME, "Failed ot get token id from request!");
            http_response_json_error ("Failed ot get lobby!");
        }
    }

    return res;

}

static HttpResponse *game_ask_answer (Server *server, DoubleList *pairs) {
    
    HttpResponse *res = NULL;

    if (server && pairs) {
        GameServerData *game_data = (GameServerData *) server->serverData;

        // get the lobby token
        const char *lobby_token = http_query_pairs_get_value (pairs, "token");
        if (lobby_token) {
            // get the lobby associated with the lobby id
            Lobby query = { .id = createString ("%s", lobby_token) };
            Lobby *lobby = lobby_get (game_data, &query);
            if (lobby) {
                // get the answer from the request
                // check if it is the correct answer
                // handle scores
                // send back feedback and next action
            }

            else {
                logMsg (stderr, ERROR, GAME, createString ("Failed to get lobby with id: %s", lobby_token));
                http_response_json_error ("Failed ot get lobby!");
            }
        }

        else {
            logMsg (stderr, ERROR, GAME, "Failed ot get token id from request!");
            http_response_json_error ("Failed ot get lobby!");
        }
    }

    return res;

}

static HttpResponse *game_ask_end (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (server && pairs) {
        GameServerData *game_data = (GameServerData *) server->serverData;

        // get the lobby token
        const char *lobby_token = http_query_pairs_get_value (pairs, "token");
        if (lobby_token) {
            // get the lobby associated with the lobby id
            Lobby query = { .id = createString ("%s", lobby_token) };
            Lobby *lobby = lobby_get (game_data, &query);
            if (lobby) {
                // end the session and return the winner name and the scoreboard as json
            }

            else {
                logMsg (stderr, ERROR, GAME, createString ("Failed to get lobby with id: %s", lobby_token));
                http_response_json_error ("Failed ot get lobby!");
            }
        }

        else {
            logMsg (stderr, ERROR, GAME, "Failed ot get token id from request!");
            http_response_json_error ("Failed ot get lobby!");
        }
    }

    return res;

}

HttpResponse *game_ask_handler (Server *server, DoubleList *pairs) {

    HttpResponse *res = NULL;

    if (pairs) {
        // get the action to perform
        const char *action = http_query_pairs_get_value (pairs, "action");
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

            else if (!strcmp (action, "create_lobby")) res = game_ask_create_lobby (server, pairs);

            else if (!strcmp (action, "join_lobby")) res = game_ask_join_lobby (server, pairs);

            else if (!strcmp (action, "leave_lobby")) res = game_ask_leave_lobby (server, pairs);

            /*** Alexa ***/

            else if (!strcmp (action, "start")) res = game_ask_start (server, pairs);

            else if (!strcmp (action, "question")) res = game_ask_question (server, pairs);

            else if (!strcmp (action, "answer")) res = game_ask_answer (server, pairs);

            else if (!strcmp (action, "end")) res = game_ask_end (server, pairs);

            else logMsg (stdout, WARNING, NO_TYPE, createString ("Found unknow ask game action %s", action));
        }

        else logMsg (stdout, ERROR, NO_TYPE, "No action provided for ask game!");
    }

    return res;

}