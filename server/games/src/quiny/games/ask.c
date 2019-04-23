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

#include "collections/dllist.h"

#include "utils/myUtils.h"
#include "utils/log.h"

/*** Questions ***/

static Question *ask_question_new (AskTopic topic, const char *question, const char *correct_answer) {

    Question *q = (Question *) malloc (sizeof (Question));
    if (q) {
        q->topic = TOPIC_GENERAL;
        q->question = str_new (question);
        q->correct_answer = str_new (correct_answer);
    }

    return q;

}

static void ask_question_delete (Question *q) {

    if (q) {
        str_delete (q->question);
        str_delete (q->correct_answer);

        free (q);
    }

}

// get the next question for an ask game
static Question *ask_question_get_next (AskGameData *ask_data) {

    Question *q = NULL;

    if (ask_data) {
        // get a new question for a game from the db based on the topic
    }

    return NULL;

}

static char *ask_question_to_json (const Question *q, size_t *len) {

    char *retval = NULL;

    if (q && len) {
        // TODO: do we have to destroy the doc?
        bson_t *doc = bson_new ();
        if (doc) {
            bson_append_utf8 (doc, "question", -1, q->question->str, q->question->len);

            retval = bson_as_json (doc, len);
        }
    }

    return NULL;

}

/*** Ask Game Data ***/

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
        // FIXME: clean up the users list without destroying the users!!!

        free (data);
    }

}

/*** Ask Game ***/

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
                // Get ask game data
                AskGameData *ask_data = (AskGameData *) lobby->game_data;
                if (ask_data) {
                    if (ask_data->active_user) 
                        dlist_insert_after (ask_data->competitors, dlist_end (ask_data->competitors), ask_data->active_user);

                    // get the next user
                    ask_data->active_user = dlist_remove_element (ask_data->competitors, dlist_start (ask_data->competitors));

                    // get the next question
                    ask_question_get_next (ask_data);

                    // send back the question and data in a json
                }

                else {
                    logMsg (stderr, ERROR, GAME, "Failed to get lobby ask game data!");
                    http_response_json_error ("Internal server error!");
                }
            }

            else {
                logMsg (stderr, ERROR, GAME, createString ("Failed to get lobby with id: %s", lobby_token));
                http_response_json_error ("Failed to get lobby!");
            }
        }

        else {
            logMsg (stderr, ERROR, GAME, "Failed ot get token id from request!");
            http_response_json_error ("Failed to get lobby!");
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
                AskGameData *ask_data = (AskGameData *) lobby->game_data;
                if (ask_data) {
                     // get the answer from the request
                    String *answer = str_new (http_query_pairs_get_value (pairs, "answer"));
                    if (answer->str) {
                        // check if it is the correct answer
                        // str_to_lower (answer);
                        if (!str_compare (answer, ask_data->current_question->correct_answer)) {
                            logMsg (stdout, SUCCESS, GAME, "Correct answer!!");

                            // handle scores
                            // send back feedback and next action
                        }

                        else {
                            logMsg (stdout, ERROR, GAME, 
                                createString ("The correct answer is: %s", ask_data->current_question->correct_answer->str));
                            // send back an error and the correct answer
                            String *error = str_new ("Respuesta incorrecta!");
                            DoubleList *jkvps = dlis_init (json_key_value_delete, NULL);
                            dlist_insert_after (jkvps, dlist_end (jkvps), json_key_value_create ("msg", error, VALUE_TYPE_STRING));
                            String *correct = str_new (ask_data->current_question->correct_answer->str);
                            dlist_insert_after (jkvps, dlist_end (jkvps), json_key_value_create ("answer", correct, VALUE_TYPE_STRING));
                            
                            size_t json_len;
                            char *json = json_create_with_pairs (jkvps, &json_len);
                            res = http_response_create (200, NULL, 0, json, json_len);

                            dlist_destroy (jkvps);
                            free (json);        // we copy the data into the response
                        }
                    }

                    else {
                        logMsg (stderr, ERROR, GAME, "No answer was provided for question!");
                        http_response_json_error ("No answer was provided!");
                    }
                }

                else {
                    logMsg (stderr, ERROR, GAME, "Failed to get lobby ask game data!");
                    http_response_json_error ("Internal server error!");
                }
            }

            else {
                logMsg (stderr, ERROR, GAME, createString ("Failed to get lobby with id: %s", lobby_token));
                http_response_json_error ("Failed to get lobby!");
            }
        }

        else {
            logMsg (stderr, ERROR, GAME, "Failed ot get token id from request!");
            http_response_json_error ("Failed to get lobby!");
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