#ifndef _GAMES_ASK_H_
#define _GAMES_ASK_H_

#include "cerver/cerver.h"
#include "cerver/http/response.h"

#include "collections/dllist.h"

extern HttpResponse *game_ask_handler (Server *server, DoubleList *pairs);

#endif