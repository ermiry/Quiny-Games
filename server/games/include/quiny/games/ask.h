#ifndef _GAMES_ASK_H_
#define _GAMES_ASK_H_

#include "cerver/http/response.h"

#include "collections/dllist.h"

extern HttpResponse *game_ask_handler (DoubleList *pairs);

#endif