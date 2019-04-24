#ifndef _GAMES_ASK_H_
#define _GAMES_ASK_H_

#include "cerver/cerver.h"
#include "cerver/http/response.h"

#include "cerver/game/score.h"

#include "mongo/mongo.h"

#include "quiny/quiny.h"

#include "collections/dllist.h"

typedef enum AskTopic {

    TOPIC_SCIENCE = 0,
    TOPIC_GEOGRAPHY = 1,
    TOPIC_HISTORY = 2,
    
    TOPIC_GENERAL = 100,

} AskTopic;

// TODO: add dummy answers for user interface
typedef struct Question {

    bson_t oid;
    String *question;
    unsigned int n_answers;
    String **answers;
    unsigned int correct_answer;    // the index of the correct answer

} Question;

typedef struct AskGameData {

    ScoreBoard *sb;
    AskTopic topic;

    // this is how we get the order of the users
    // this list works as a queue
    DoubleList *competitors; 
    User *active_user;
    Question *current_question;     // the current active question   
    DoubleList *question_log;    

} AskGameData;

extern HttpResponse *game_ask_handler (Server *server, DoubleList *pairs);

#endif