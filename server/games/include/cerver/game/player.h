#ifndef _CERVER_PLAYER_H_
#define _CERVER_PLAYER_H_

typedef uint16_t PlayerId;

typedef struct Player {

	struct _Client *client;		// client network data associated to this player

	PlayerId id;
	bool inLobby;

	// PlayerInput input;
	// u32 inputSequenceNum;
	// TimeSpec lastInputTime;

	bool alive;

	// 15/11/2018 -- we spec the player to be ecs based
	// the server admin can add its own components
	void *components;

} Player;

#endif