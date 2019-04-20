#ifndef _CERVER_LOBBY_H_
#define _CERVER_LOBBY_H_

#include "types/myTypes.h"

#include "game.h"
#include "player.h"

#include "collections/avl.h"

#include "utils/objectPool.h"

struct _Player;
// enum _GameType;

struct _GameSettings {

    // FIXME: set init function for diffrent game types
	// enum _GameType gameType;

	u8 playerTimeout; 	// in seconds.
	u8 fps;

	u8 minPlayers;
	u8 maxPlayers;

	// duration?

};

typedef struct _GameSettings GameSettings;

struct _Lobby {

	unsigned int id;

	// GameSettings *settings;
	void *game_settings;
	Action delete_lobby_game_settings;

	bool inGame;

	struct _Player *owner;				// the client that created the lobby -> he has higher privileges

	bool isRunning;						// lobby is listening for player packets

	// 04/11/2018 -- lets try this and see how it goes - intended to also work for a bigger 
	// lobby with more players in it
	AVLTree *players;							// players inside the lobby -> reference to the main player avl
	unsigned int max_players;
	unsigned int current_players;
    struct pollfd *players_fds;     			
    u16 players_nfds;                           // n of active fds in the pollfd array
    bool compress_players;              		// compress the fds array?
    u32 pool_timeout;    

	// the server admin can add its server specific data types
	void *game_data;
	Action delete_lobby_game_data;

	// 21/11/2018 - we put this here to avoid race conditions if we put it on
	// the server game data
	// Pool *gamePacketsPool;

};

typedef struct _Lobby Lobby;

// sets the lobby settings and a function to delete it
extern void lobby_set_game_settings (Lobby *lobby, void *game_settings, Action delete_game_settings);
// sets the lobby game data and a function to delete it
extern void lobby_set_game_data (Lobby *lobby, void *game_data, Action delete_lobby_game_data);

// lobby constructor, it also initializes basic lobby data
extern Lobby *lobby_new (Server *server, unsigned int max_players);
// deletes a lobby for ever -- called when we teardown the server
// we do not need to give any feedback to the players if there is any inside
extern void lobby_delete (void *ptr);
extern int lobby_comparator (void *one, void *two);

// create a list to manage the server lobbys
// called when we init the game server
extern u8 game_init_lobbys (GameServerData *game_data, u8 n_lobbys);

/*** Player interaction ***/

// creates a new lobby and inits his values with an owner
extern Lobby *lobby_create (Server *server, Player *owner, unsigned int max_players);
// called by a registered player that wants to join a lobby on progress
// the lobby model gets updated with new values
extern u8 lobby_join (Lobby *lobby, Player *player);
// called when a player requests to leave the lobby
extern u8 lobby_leave (Lobby *lobby, Player *player);

#endif