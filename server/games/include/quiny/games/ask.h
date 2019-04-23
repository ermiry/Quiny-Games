#ifndef _GAMES_ASK_H_
#define _GAMES_ASK_H_

#include "cerver/cerver.h"
#include "cerver/http/response.h"

#include "cerver/game/score.h"

#include "quiny/quiny.h"

#include "collections/dllist.h"

typedef enum AskTopic {

    TOPIC_SCIENCE,
    TOPIC_GEOGRAPHY,
    TOPIC_HISTORY,
    
    TOPIC_GENERAL,

} AskTopic;

// TODO: add dummy answers for user interface
typedef struct Question {

    AskTopic topic;
    String *question;
    String *correct_answer;

} Question;

typedef struct AskGameData {

    ScoreBoard *sb;
    AskTopic topic;

    // this is how we get the order of the users
    // this list works as a queue
    DoubleList *competitors; 
    User *active_user;
    Question *current_question;     // the current active question       

} AskGameData;

extern HttpResponse *game_ask_handler (Server *server, DoubleList *pairs);

#endif