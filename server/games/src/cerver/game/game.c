#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <poll.h>
#include <errno.h>

#include <stdarg.h>

#include "types/myTypes.h"

#include "cerver/game/game.h"
#include "cerver/game/player.h"
#include "cerver/game/lobby.h"

#include "collections/dllist.h"
#include "collections/avl.h"
#include "collections/htab.h"

#include "utils/objectPool.h"

#include "utils/myUtils.h"
#include "utils/config.h"
#include "utils/log.h"

GamePacketInfo *newGamePacketInfo (Server *server, Lobby *lobby, Player *player, 
    char *packetData, size_t packetSize);

/*** GAME SERVER ***/

#pragma region GAME SERVER

// add a function to load game data like from a db
void gs_set_loadGameData (Server *server, Func loadData) {

    if (server) {
        if (server->type != GAME_SERVER) {
            logMsg (stderr, ERROR, SERVER, 
                "Can't add a load game data func. Server of incorrect type.");
            return;
        }

        GameServerData *gameData = (GameServerData *) server->serverData;
        if (!gameData) {
            logMsg (stderr, ERROR, SERVER, "NULL game data in the game server!");
            return;
        }

        gameData->loadGameData = loadData;
    }

}

void gs_set_deleteGameData (Server *server, Func deleteData) {

    if (server) {
        if (server->type != GAME_SERVER) {
            logMsg (stderr, ERROR, SERVER, 
                "Can't add a delete game data func. Server of incorrect type.");
            return;
        }

        GameServerData *gameData = (GameServerData *) server->serverData;
        if (!gameData) {
            logMsg (stderr, ERROR, SERVER, "NULL game data in the game server!");
            return;
        }

        gameData->deleteGameData = deleteData;
    }

}

// TODO: 13/11/2018 - do we still need a hash table for a better implementation?
// we need the game types enum to be 0 indexed and to be in order!
// adds a game init function to use with a game type
// TODO: 13/11/2018 -- we need to support the admin to add new game modes without
// changing the original GameType enum
void gs_add_gameInit (struct _Server *server, GameType gameType, delegate gameInit) {

    if (server) {
        if (server->type != GAME_SERVER) {
            logMsg (stderr, ERROR, SERVER, "Can't add a game init func. Server of incorrect type.");
            return;
        }

        GameServerData *gameData = (GameServerData *) server->serverData;
        if (!gameData) {
            logMsg (stderr, ERROR, SERVER, "NULL game data in the game server!");
            return;
        }

        // the first time we add a function, we create the array
        if (!gameData->gameInitFuncs) {
            gameData->gameInitFuncs = (delegate *) calloc (1, sizeof (delegate));
            gameData->gameInitFuncs[gameType] = gameInit;
            gameData->n_gameInits++;
        }

        // realloc the array and add the new function at the end
        else {
            gameData->gameInitFuncs = 
                (delegate *) realloc (gameData->gameInitFuncs, 
                sizeof (delegate) * gameData->n_gameInits + 1);
            gameData->gameInitFuncs[gameType] = gameInit;
            gameData->n_gameInits++;
        }
    }

}

void gs_set_lobbyDeleteGameData (Server *server, Action deleteData) {

     if (server) {
        if (server->type != GAME_SERVER) {
            logMsg (stderr, ERROR, SERVER, 
                "Can't add a lobby delete game data func. Server of incorrect type.");
            return;
        }

        GameServerData *gameData = (GameServerData *) server->serverData;
        if (!gameData) {
            logMsg (stderr, ERROR, SERVER, "NULL game data in the game server!");
            return;
        }

        gameData->deleteLobbyGameData = deleteData;
    }

}

void broadcastToAllPlayers (AVLNode *, Server *, void *, size_t);

u8 destroyLobby (Server *server, Lobby *lobby);

// cleans up all the game structs like lobbys and in game data set by the admin
// if there are players connected, it sends a server teardown packet
u8 destroyGameServer (Server *server) {

    if (server) {
        #ifdef CERVER_DEBUG
            logMsg (stdout, DEBUG_MSG, SERVER, "Destroying game server data...");
        #endif

        GameServerData *gameData = (GameServerData *) server->serverData;
        if (gameData) {
            // first destroy the current lobbys, stoping any ongoing game and sending the players
            // to the main server players avl structure
            while (dlist_size (gameData->currentLobbys) > 0) 
                destroyLobby (server, (Lobby *) (dlist_start (gameData->currentLobbys)));

            // destroy all lobbys
            dlist_clean (gameData->currentLobbys);
            pool_clear (gameData->lobbyPool);

            #ifdef DEBUG
                logMsg (stdout, DEBUG_MSG, SERVER, "Done clearing server lobbys.");
            #endif

            // send server destroy packet to all the players
            size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
            void *packet = generatePacket (SERVER_PACKET, packetSize);
            if (packet) {
                char *end = packet + sizeof (PacketHeader);
                RequestData *req = (RequestData *) end;
                req->type = SERVER_TEARDOWN;

                // send the packet to players outside the lobby
                broadcastToAllPlayers (gameData->players->root, server, packet, packetSize);

                free (packet);
            }

            else logMsg (stderr, ERROR, PACKET, "Failed to create server teardown packet!");

            #ifdef DEBUG
                logMsg (stdout, DEBUG_MSG, SERVER, "Done sending server teardown packet to players.");
            #endif

            // disconnect all players from the server
                // -> close connections -> take them out of server poll structures
            // 20/11/2108 - this is done in the server teardown function because cleints
            // and players share the same poll structure

            // destroy players
            if (gameData->players) 
                avl_clearTree (&gameData->players->root, gameData->players->destroy);
            pool_clear (gameData->playersPool);

            #ifdef DEBUG
                logMsg (stdout, DEBUG_MSG, SERVER, "Done clearing server players.");
            #endif

            // destroy game specific data set by the game admin
            if (gameData->deleteGameData) gameData->deleteGameData ();

            // remove any other data or values in the game server...
            if (gameData->gameSettingsConfig) 
                config_destroy (gameData->gameSettingsConfig);
            gameData->loadGameData = NULL;
            gameData->deleteGameData = NULL;

            free (gameData);

            return 0;
        }

        else logMsg (stderr, ERROR, SERVER, "Game server doesn't have a reference to game data!");
    }

    else logMsg (stderr, ERROR, SERVER, "Can't destroy a NULL game server!");

    return 1;

}

#pragma endregion

// FIXME:
#pragma region GAME SERIALIZATION 

// TODO: what info do we want to send?
void serialize_player (SPlayer *splayer, Player *player) {

    if (splayer && player) {
        
    }

}

// FIXME:
// TODO: what treatment do we want to give the serialized data?

// traverse each player in the lobby's avl and serialize it
void serialize_lobby_players (AVLNode *node, SArray *sarray) {

    if (node && sarray) {
        serialize_lobby_players (node->right, sarray);

        if (node->id) {

        }

        serialize_lobby_players (node->left, sarray);
    }

}

#pragma endregion

/*** SCORE ***/

// this should be part of the cerver framework

// 15/11/2018 - starting to implement a easy to use score manager for any popouse game
// implementing a C# Dictionary <string, Dictionary <string, int>> using hastables

// FIXME: 17/11/2018 -- we need a way of retrieving the scores in order!!

// FIXME: we need to sreliaze and send the scores to the players in the best way

// FIXME: we still need to fix some functions...

#pragma region SCORE 

// 16/11/2018 -- we can now create a scoreboard and pass variable num of score types
// to be added each time we add a new player!!
ScoreBoard *game_score_new (u8 playersNum, u8 scoresNum, ...) {

    ScoreBoard *sb = (ScoreBoard *) malloc (sizeof (ScoreBoard));
    if (sb) {
        va_list valist;
        va_start (valist, scoresNum);

        sb->scoresNum = scoresNum;
        sb->scoreTypes = (char **) calloc (scoresNum, sizeof (char *));
        char *temp = NULL;
        for (int i = 0; i < scoresNum; i++) {
            temp = va_arg (valist, char *);
            sb->scoreTypes[i] = (char *) calloc (strlen (temp) + 1, sizeof (char));
            strcpy (sb->scoreTypes[i], temp);
        }

        sb->registeredPlayers = 0;
        sb->scores = htab_init (playersNum > 0 ? playersNum : DEFAULT_SCORE_SIZE,
             NULL, NULL, NULL, NULL);

        va_end (valist);
    }

    return sb;

}

// destroy the scoreboard
void game_score_destroy (ScoreBoard *sb) {

    if (sb) {
        if (sb->scores) htab_destroy (sb->scores);
        if (sb->scoreTypes) {
            for (int i = 0; i < sb->scoresNum; i++) 
                if (sb->scoreTypes[i]) free (sb->scoreTypes[i]);
            
        }
    }

}

// FIXME:
// add a new score type to all current players
void game_score_add_scoreType (ScoreBoard *sb, char *newScore) {

    if (sb && newScore) {
        // first check if we have that score type
        bool found = false;
        for (int i = 0; i < sb->scoresNum; i++) {
            if (!strcmp (sb->scoreTypes[i], newScore)) {
                found = true;
                break;
            }
        }

        if (found) {
            // add the new score to all the players
            // FIXME:

            // add the score to the scoretypes
            sb->scoreTypes = (char **) realloc (sb->scores, sb->scoresNum);
            if (sb->scoreTypes) {
                sb->scoreTypes[sb->scoresNum] = (char *) calloc (strlen (newScore) + 1, sizeof (char));
                strcpy (sb->scoreTypes[sb->scoresNum], newScore);
                sb->scoresNum++;
            } 
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                "Can't add score type! It already exists in the scoreboard.");
            #endif
        }
    }

}

// FIXME:
// remove a score type from all current players
u8 game_score_remove_scoreType (ScoreBoard *sb, char *oldScore) {

    if (sb && oldScore) {
        // first check if we have that score type
        int found = -1;
        for (int i = 0; i < sb->scoresNum; i++) {
            if (!strcmp (sb->scoreTypes[i], oldScore)) {
                found = i;
                break;
            }
        }

        if (found > 0) {
            // remove the score from all the players
            // FIXME:

            // manually copy old scores array and delete it
            // this is done because we allocate exat space for each score type!
            char **new_scoreTypes = (char **) calloc (sb->scoresNum - 1, sizeof (char *));
            int i = 0;
            while (i < found) {
                new_scoreTypes[i] = (char *) calloc (strlen (sb->scoreTypes[i]) + 1, sizeof (char));
                strcpy (new_scoreTypes[i], sb->scoreTypes[i]);
                i++;
            }

            for (i = found + 1; i < sb->scoresNum; i++) {
                new_scoreTypes[i - 1] = (char *) calloc (strlen (sb->scoreTypes[i]) + 1, sizeof (char));
                strcpy (new_scoreTypes[i - 1], sb->scoreTypes[i]);
            }

            sb->scoresNum--;

            return 0;
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Can't remove %s scoretype, doesn't exist in the scoreboard!"));
            #endif
            return 1;
        }
    }

}

// adds a new player to the score table
u8 game_score_add_player (ScoreBoard *sb, char *playerName) {

    if (sb && playerName) {
        if (htab_contains_key (sb->scores, playerName, sizeof (playerName))) {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Scores table already contains player: %s", playerName));
            #endif
            return 1;
        }

        // associate the player with his own scores dictionary
        else {
            Htab *newHt = htab_init (sb->scoresNum, NULL, NULL, NULL, NULL);
            if (newHt) {
                if (!htab_insert (sb->scores, playerName, sizeof (playerName), newHt, sizeof (Htab))) {
                    // insert each score type in the player's dictionary
                    u32 zero = 0;
                    for (int i = 0; i < sb->scoresNum; i++) 
                        htab_insert (newHt, sb->scoreTypes[i], 
                            sizeof (sb->scoreTypes[i]), &zero, sizeof (int));

                    sb->registeredPlayers++;

                    return 0;   // success adding new player and its scores
                }
            }
        }
    }

    return 1;

}

// FIXME: how do we get the player name?? the server is not interested in knowing a playe's name
// inside his player structure, it is the admin reponsability to determine how to get 
// the player's name
void game_score_add_lobby_players (ScoreBoard *sb, AVLNode *playerNode) {

    if (sb && playerNode) {
        game_score_add_lobby_players (sb, playerNode->right);

        // if (playerNode->id) game_score_add_player (sb, ((Player *) playerNode->id));

        game_score_add_lobby_players (sb, playerNode->left);
    }

}

// removes a player from the score table
u8 game_score_remove_player (ScoreBoard *sb, char *playerName) {

    if (sb && playerName) {
        if (htab_contains_key (sb->scores, playerName, sizeof (playerName))) {
            size_t htab_size = sizeof (Htab);
            void *htab = htab_getData (sb->scores, playerName, sizeof (playerName), &htab_size);
            
            // destroy player's scores htab
            if (htab) htab_destroy ((Htab *) htab);

            // remove th player from the scoreboard
            htab_remove (sb->scores, playerName, sizeof (playerName));

            sb->registeredPlayers--;
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Scores table doesn't contains player: %s", playerName));
            #endif
        }
    }

    return 1;   // error

}

// sets the score's value, negative values not allowed
void game_score_set (ScoreBoard *sb, char *playerName, char *scoreType, i32 value) {

    if (sb && playerName && scoreType) {
        size_t htab_size = sizeof (Htab);
        void *playerScores = htab_getData (sb->scores, playerName, sizeof (playerName), &htab_size);
        if (playerScores) {
            size_t int_size = sizeof (unsigned int);
            void *currValue = htab_getData ((Htab *) playerScores, 
                scoreType, sizeof (scoreType), &int_size);

            // replace the old value with the new one
            if (currValue) {
                u32 *current = (u32 *) currValue;
                if (value > 0) *current = value;
                else *current = 0;
            }
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Player: %s has not scores in the table!", playerName));
            #endif
        }
    }

}

// gets the value of the player's score type
i32 game_score_get (ScoreBoard *sb, char *playerName, char *scoreType) {

    if (sb && playerName && scoreType) {
        size_t htab_size = sizeof (Htab);
        void *playerScores = htab_getData (sb->scores, playerName, sizeof (playerName), &htab_size);
        if (playerScores) {
            size_t int_size = sizeof (unsigned int);
            void *value = htab_getData ((Htab *) playerScores, 
                scoreType, sizeof (scoreType), &int_size);

            if (value) {
                u32 *retval = (u32 *) value;
                return *retval;
            }
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Player: %s has not scores in the table!", playerName));
            #endif
        }
    }

    return -1;  // error

}

// updates the value of the player's score type, clamped to 0
void game_score_update (ScoreBoard *sb, char *playerName, char *scoreType, i32 value) {

    if (sb && playerName && scoreType) {
        size_t htab_size = sizeof (Htab);
        void *playerScores = htab_getData (sb->scores, playerName, sizeof (playerName), &htab_size);
        if (playerScores) {
            size_t int_size = sizeof (unsigned int);
            void *currValue = htab_getData ((Htab *) playerScores, 
                scoreType, sizeof (scoreType), &int_size);
            
            // directly update the score value adding the new value
            if (value) {
                u32 *current = (u32 *) currValue;
                if ((*current += value) < 0) *current = 0;
                else *current += value;
            }
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Player: %s has not scores in the table!", playerName));
            #endif
        }
    }

}

// reset all the player's scores
void game_score_reset (ScoreBoard *sb, char *playerName) {

    if (sb && playerName) {
        size_t htab_size = sizeof (Htab);
        void *data = htab_getData (sb->scores, playerName, sizeof (playerName), &htab_size);
        if (data) {
            Htab *playerScores = (Htab *) data;
            void *scoreData = NULL;
            size_t score_size = sizeof (u32);
            for (int i = 0; i < sb->scoresNum; i++) {
                scoreData = htab_getData (playerScores, 
                    sb->scoreTypes[i], sizeof (sb->scoreTypes[i]), &score_size);

                if (scoreData) {
                    u32 *current = (u32 *) scoreData;
                    *current = 0;
                }
            }
        }

        else {
            #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, 
                createString ("Scores table already contains player: %s", playerName));
            #endif
        }
    }

}

#pragma endregion

/*** GAME THREAD ***/

#pragma region GAME LOGIC

// TODO: 13/11/2018 - do we want to start the game logic in a new thread?
    // remeber that the lobby is listening in its own thread that can handle imput logic
    // but what about a thread in the lobby for listenning and another for game logic and sending packets?

// FIXME: how do we hanlde the creation of different types of games?
// the game server starts a new game with the function set for the dessired game type
u8 gs_startGame (Server *server, Lobby *lobby) {

    if (server && lobby) {
        GameServerData *gameData = (GameServerData *) server->serverData;

        if (!gameData->gameInitFuncs) {
            logMsg (stderr, ERROR, GAME, "Init game functions not set!");
            return 1;
        }

        // delegate temp = gameData->gameInitFuncs[lobby->settings->gameType];
        /* if (temp) {
            // 14/11/2018 -- al game functions will get a reference to the server and lobby
            // where they are requested to init
            ServerLobby *sl = (ServerLobby *) malloc (sizeof (ServerLobby));
            sl->server = server;
            sl->lobby = lobby;

            #ifdef DEBUG
                logMsg (stdout, DEBUG_MSG, GAME, "Starting the game...");
            #endif

            // we expect the game function to sync players and send game packets directly 
            // using the framework
            if (!temp (sl)) {
                #ifdef DEBUG
                    logMsg (stdout, SUCCESS, GAME, "A new game has started!");
                #endif
                return 0;
            }

            else {
                #ifdef DEBUG
                    logMsg (stderr, ERROR, GAME, "Failed to start a new game!");
                #endif
                return 1;
            }
        }
        
        else {
            logMsg (stderr, ERROR, GAME, "No init function set for the desired game type!");
            return 1;
        } */
        
    }

    else return 1;

}

// FIXME:
// stops the current game in progress
// called when the players don't finish the current game
void stopGame (void) {

    // logic to stop the current game in progress

}

// FIXME:
// called when the win condition has been reached inside a lobby
void endGame (void) {

    // logic to stop the current game in the server
    // send feedback to the players that the game has finished
    // send players the scores and leaderboard
    // the lobby owner can retry the game or exit to the lobby screen

}

#pragma endregion

// FIXME: where do we want to put this?
/*** REQUESTS FROM INSIDE THE LOBBY ***/

// a player wants to send a message in the lobby chat
void msgtoLobbyPlayers () {

}

/*** GAME UPDATES ***/

// handle a player input, like movement -> TODO: we need to check for collisions between enemies
void handlePlayerInput () {

}

#pragma region GAME PACKETS

// FIXME: serialize players inside the lobby
// creates a lobby packet with the passed lobby info
void *createLobbyPacket (RequestType reqType, Lobby *lobby, size_t packetSize) {

    void *packetBuffer = malloc (packetSize);
    void *begin = packetBuffer;
    char *end = begin; 

    PacketHeader *header = (PacketHeader *) end;
    initPacketHeader (header, GAME_PACKET, packetSize); 
    end += sizeof (PacketHeader);

    RequestData *reqdata = (RequestData *) end;
    reqdata->type = reqType;
    end += sizeof (RequestType);

    // serialized lobby data
    SLobby *slobby = (SLobby *) end;
    end += sizeof (SLobby);

    // slobby->settings.gameType = lobby->settings->gameType;
    slobby->settings.fps = lobby->settings->fps;
    slobby->settings.minPlayers = lobby->settings->minPlayers;
    slobby->settings.maxPlayers = lobby->settings->maxPlayers;
    slobby->settings.playerTimeout = lobby->settings->playerTimeout;

    slobby->inGame = lobby->inGame;

    // FIXME: 
    // we serialize the players using a vector
    // slobby->players.n_elems = 0;

    return begin;

}

// sends a lobby update packet to all the players inside a lobby
void sendLobbyPacket (Server *server, Lobby *lobby) {

    if (lobby) {
        size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData) +
            sizeof (SLobby) + lobby->players_nfds * sizeof (SPlayer);
        void *lobbyPacket = createLobbyPacket (LOBBY_UPDATE, lobby, packetSize);
        if (lobbyPacket) {
            broadcastToAllPlayers (lobby->players->root, server, lobbyPacket, packetSize);
            free (lobbyPacket);
        }

        else logMsg (stderr, ERROR, PACKET, "Failed to create lobby update packet!");
    }

}

/*** Prototypes of public game server functions ***/

/*** reqs outside a lobby ***/
void gs_createLobby (Server *, Client *, i32, GameType);
void gs_joinLobby (Server *, Client *, GameType);

/*** reqs from inside a lobby ***/
void gs_leaveLobby (Server *, Player *, Lobby *);
void gs_initGame (Server *, Player *, Lobby *);
void gs_sendMsg (Server *, Player *, Lobby *, char *msg);

// this is called from the main poll in a new thread
void gs_handlePacket (PacketInfo *pack_info) {

    logMsg (stdout, DEBUG_MSG, GAME, "gs_handlePacket ()");

    RequestData *reqData = (RequestData *) (pack_info->packetData + sizeof (PacketHeader));
    switch (reqData->type) {
        // TODO: get the correct game type from the packet
        case LOBBY_CREATE: 
            gs_createLobby (pack_info->server, pack_info->client, pack_info->clientSock, ARCADE); break;
        case LOBBY_JOIN: gs_joinLobby (pack_info->server, pack_info->client, ARCADE); break;
        default: break;
    }

}

GamePacketInfo *newGamePacketInfo (Server *server, Lobby *lobby, Player *player, 
    char *packetData, size_t packetSize) {

    if (server && lobby && player && packetData && (packetSize > 0)) {
        GamePacketInfo *info = NULL;

        if (lobby->gamePacketsPool) {
            info = (GamePacketInfo *) pool_pop (lobby->gamePacketsPool);
            if (!info) info = (GamePacketInfo *) malloc (sizeof (GamePacketInfo));
        }

        else info = (GamePacketInfo *) malloc (sizeof (GamePacketInfo));

        if (info) {
            info->server = server;
            info->lobby = lobby;
            info->player = player;
            strcpy (info->packetData, packetData);
            info->packetSize = packetSize;

            return info;
        }
    }

    return NULL;

}

void deleteGamePacketInfo (void *data) {

    if (data) {
        GamePacketInfo *info = (GamePacketInfo *) info;

        info->server = NULL;
        info->lobby = NULL;
        info->player = NULL;

        free (info);
    }

}

// handles all valid requests a player can make from inside a lobby
void handleGamePacket (void *data) {

    if (data) {
        GamePacketInfo *packet = (GamePacketInfo *) data;
        if (!checkPacket (packet->packetSize, packet->packetData, GAME_PACKET)) {
            RequestData *rdata = (RequestData *) packet->packetData + sizeof (PacketHeader);

            switch (rdata->type) {
                case LOBBY_LEAVE: 
                    gs_leaveLobby (packet->server, packet->player, packet->lobby); break;

                case GAME_START: 
                    gs_initGame (packet->server, packet->player, packet->lobby); break;
                case GAME_INPUT_UPDATE:  break;
                case GAME_SEND_MSG: break;

                // dispose the packet -> send it to the pool
                default: break;
            }

            // after we have use the packet, send it to the pool
            pool_push (packet->lobby->gamePacketsPool, packet);
        }

        // invalid packet -> dispose -> send it to the pool
        else pool_push (packet->lobby->gamePacketsPool, packet);
    }

}

#pragma endregion

/*** PUBLIC FUNCTIONS ***/

// These functions are the ones that handle the request from the client/player
// They provide an interface to interact with the server
// Apart from this functions, all other logic must be PRIVATE to the client/player

#pragma region PUBLIC FUNCTIONS

/*** FROM OUTSIDE A LOBBY ***/

// request from a from client to create a new lobby 
void gs_createLobby (Server *server, Client *client, i32 sock_fd, GameType gameType) {

    if (server && client) {
        #ifdef CERVER_DEBUG
            logMsg (stdout, DEBUG_MSG, GAME, "Creating a new lobby...");
        #endif

        GameServerData *gameData = (GameServerData *) server->serverData;

        // check if the client is associated with a player
        Player *owner = getPlayerBySock (gameData->players->root, client->active_connections[0]);

        // create new player data for the client
        if (!owner) {
            owner = newPlayer (gameData->playersPool, client);
            player_registerToServer (server, owner);
        } 

        // check that the owner isn't already in a lobby or game
        if (owner->inLobby) {
            #ifdef DEBUG
            logMsg (stdout, DEBUG_MSG, GAME, "A player inside a lobby wanted to create a new lobby.");
            #endif
            if (sendErrorPacket (server, sock_fd, client->address, 
                ERR_CREATE_LOBBY, "Player is already in a lobby!")) {
                #ifdef DEBUG
                logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                #endif
            }
            return;
        }

        Lobby *lobby = createLobby (server, owner, gameType);
        if (lobby) {
            #ifdef DEBUG
                logMsg (stdout, SUCCESS, GAME, "New lobby created!");
            #endif 

            // send the lobby info to the owner -- we only have one player inside the lobby
            size_t lobby_packet_size = sizeof (PacketHeader) + sizeof (RequestData) +
                sizeof (SLobby) + lobby->players_nfds * sizeof (SPlayer);
            void *lobby_packet = createLobbyPacket (LOBBY_UPDATE, lobby, lobby_packet_size);
            if (lobby_packet) {
                if (server_sendPacket (server, sock_fd, owner->client->address, 
                    lobby_packet, lobby_packet_size))
                    logMsg (stderr, ERROR, PACKET, "Failed to send back lobby packet to owner!");
                else logMsg (stdout, SUCCESS, PACKET, "Sent lobby packet to owner!");
                free (lobby_packet);
            }

            // TODO: do we wait for an ack packet from the client?
        }

        // there was an error creating the lobby
        else {
            logMsg (stderr, ERROR, GAME, "Failed to create a new game lobby.");
            // send feedback to the player
            sendErrorPacket (server, sock_fd, client->address, ERR_SERVER_ERROR, 
                "Game server failed to create new lobby!");
        }
    }

}

// FIXME: use the custom action find lobby set by the admin
// TODO: in a larger game, a player migth wanna join a lobby with his friend
// or we would have an algorithm to find a suitable lobby based on player stats
// as of 10/11/2018 -- a player just joins the first lobby that we find that is not full
void gs_joinLobby (Server *server, Client *client, GameType gameType) {

    if (server && client) {
        GameServerData *gameData = (GameServerData *) server->serverData;

        // check if the client is associated with a player
        // Player *player = getPlayerBySock (gameData->players, client->clientSock);
        Player *player;

        // create new player data for the client
        if (!player) {
            player = newPlayer (gameData->playersPool, client);
            player_registerToServer (server, player);
        } 

        // check that the owner isn't already in a lobby or game
        if (player->inLobby) {
            #ifdef DEBUG
            logMsg (stdout, DEBUG_MSG, GAME, "A player inside a lobby wanted to join a new lobby.");
            #endif
            // FIXME:
            /* if (sendErrorPacket (server, player->client, ERR_CREATE_LOBBY, "Player is already in a lobby!")) {
                #ifdef DEBUG
                logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                #endif
            } */
            return;
        }

        Lobby *lobby = findLobby (server);

        if (lobby) {
            // add the player to the lobby
            if (!joinLobby (server, lobby, player)) {
                // the player joined successfully
                #ifdef DEBUG
                    logMsg (stdout, DEBUG_MSG, GAME, "A new player has joined the lobby");
                #endif
            }

            // errors are handled inisde the join lobby function
        }

        // FIXME:
        // failed to find a lobby, so create a new one
        // else gs_createLobby (server, client, gameType);
    }

}

/*** FROM INSIDE A LOBBY ***/

void gs_leaveLobby (Server *server, Player *player, Lobby *lobby) {

    if (server && player && lobby) {
        GameServerData *gameData = (GameServerData *) server->serverData;

        if (player->inLobby) {
            if (!leaveLobby (server, lobby, player)) {
                #ifdef DEBUG
                    logMsg (stdout, DEBUG_MSG, GAME, "PLayer left the lobby successfully!");
                #endif
            }

            else {
                #ifdef DEBUG
                logMsg (stdout, DEBUG_MSG, GAME, "There was a problem with a player leaving a lobby!");
                #endif
                // FIXME:
                /* if (sendErrorPacket (server, player->client, ERR_LEAVE_LOBBY, "Problem with player leaving the lobby!")) {
                    #ifdef DEBUG
                    logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                    #endif
                } */
            }
        }

        else {
            #ifdef DEBUG
            logMsg (stdout, DEBUG_MSG, GAME, "A player tries to leave a lobby but he is not inside one!");
            #endif
            // FIXME:
            /* if (sendErrorPacket (server, player->client, ERR_LEAVE_LOBBY, "Player is not inside a lobby!")) {
                #ifdef DEBUG
                logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                #endif
            } */
        }
    }

}

void gs_initGame (Server *server, Player *player, Lobby *lobby) {

    if (server && player && lobby) {
        GameServerData *gameData = (GameServerData *) server->serverData;

        if (player->inLobby) {
            if (lobby->owner->id == player->id) {
                if (lobby->players_nfds >= lobby->settings->minPlayers) {
                    if (gs_startGame (server, lobby)) {
                        logMsg (stderr, ERROR, GAME, "Failed to start a new game!");
                        // send feedback to the players
                        size_t packetSize = sizeof (PacketHeader) + sizeof (ErrorData);
                        void *errpacket = generateErrorPacket (ERR_GAME_INIT, 
                            "Server error. Failed to init the game.");
                        if (errpacket) {
                            broadcastToAllPlayers (lobby->players->root, server, errpacket, packetSize);
                            free (errpacket);
                        }

                        else {
                            #ifdef DEBUG
                            logMsg (stderr, ERROR, PACKET, 
                                "Failed to create & send error packet to client!");
                            #endif
                        }
                    }

                    // upon success, we expect the start game func to send the packages
                    // we start the game on the new thread
                }

                else {
                    #ifdef DEBUG
                    logMsg (stdout, WARNING, GAME, "Need more players to start the game.");
                    #endif
                    // FIXME: select client socket!!
                    /* if (sendErrorPacket (server, player->client, ERR_GAME_INIT, 
                        "We need more players to start the game!")) {
                        #ifdef DEBUG
                        logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                        #endif
                    } */
                }
            }

            else {
                #ifdef DEBUG
                logMsg (stdout, WARNING, GAME, "Player is not the lobby owner.");
                #endif
                // FIXME:
                /* if (sendErrorPacket (server, player->client, ERR_GAME_INIT, 
                    "Player is not the lobby owner!")) {
                    #ifdef DEBUG
                    logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                    #endif
                } */
            }
        }

        else {
            #ifdef DEBUG
            logMsg (stdout, WARNING, GAME, "Player must be inside a lobby and be the owner to start a game.");
            #endif
            // FIXME:
            /* if (sendErrorPacket (server, player->client, ERR_GAME_INIT, 
                "The player is not inside a lobby!")) {
                #ifdef DEBUG
                logMsg (stderr, ERROR, PACKET, "Failed to create & send error packet to client!");
                #endif
            } */
        }
    }

}

// TODO:
// a player wants to send a msg to the players inside the lobby
void gs_sendMsg (Server *server, Player *player, Lobby *lobby, char *msg) {

    if (server && player && lobby && msg) {

    }

}

#pragma endregion