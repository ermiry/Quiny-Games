#ifndef _CERVER_LOBBY_H_
#define _CERVER_LOBBY_H_

#include "types/myTypes.h"

#include "game.h"
#include "player.h"

#include "collections/avl.h"

#include "utils/objectPool.h"

struct _GameSettings {

	GameType gameType;

	u8 playerTimeout; 	// in seconds.
	u8 fps;

	u8 minPlayers;
	u8 maxPlayers;

	// duration?

};

typedef struct _GameSettings GameSettings;

struct _Lobby {

	GameSettings *settings;
	bool inGame;

	Player *owner;				// the client that created the lobby -> he has higher privileges

	bool isRunning;				// lobby is listening for player packets

	// 04/11/2018 -- lets try this and see how it goes - intended to also work for a bigger 
	// lobby with more players in it
	AVLTree *players;							// players inside the lobby
    struct pollfd players_fds[4];     			// 04/11/2018 - 4 max players in lobby
    u16 players_nfds;                           // n of active fds in the pollfd array
    bool compress_players;              		// compress the fds array?
    u32 pollTimeout;    

	// the server admin can add its server specific data types
	void *gameData;
	Action deleteLobbyGameData;

	// 21/11/2018 - we put this here to avoid race conditions if we put it on
	// the server game data
	Pool *gamePacketsPool;

};

typedef struct _Lobby Lobby;

#endif