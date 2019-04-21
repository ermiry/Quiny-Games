#ifndef _GAMES_ASK_H_
#define _GAMES_ASK_H_

#include "cerver/cerver.h"
#include "cerver/http/response.h"

#include "cerver/game/score.h"

#include "collections/dllist.h"

typedef enum AskTopic {

    TOPIC_SCIENCE,
    TOPIC_GEOGRAPHY,
    TOPIC_HISTORY,
    
    TOPIC_GENERAL,

} AskTopic;

typedef struct AskGameData {

    ScoreBoard *sb;
    AskTopic topic;

} AskGameData;

extern HttpResponse *game_ask_handler (Server *server, DoubleList *pairs);

#endif