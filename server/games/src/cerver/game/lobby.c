#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <poll.h>
#include <errno.h>

#include "types/myTypes.h"

#include "cerver/game/game.h"
#include "cerver/game/player.h"

#include "collections/dllist.h"
#include "collections/avl.h"

#include "utils/objectPool.h"

#include "utils/myUtils.h"
#include "utils/config.h"
#include "utils/log.h"

void *createLobbyPacket (RequestType reqType, struct _Lobby *lobby, size_t packetSize);
void sendLobbyPacket (Server *server, Lobby *lobby);

/*** Lobby Configuration ***/

// sets the lobby settings and a function to delete it
void lobby_set_game_settings (Lobby *lobby, void *game_settings, Action delete_game_settings) {

    if (lobby) {
        lobby->game_settings = game_settings;
        lobby->delete_lobby_game_settings = delete_game_settings;
    }

}

// sets the lobby game data and a function to delete it
void lobby_set_game_data (Lobby *lobby, void *game_data, Action delete_lobby_game_data) {

    if (lobby) {
        lobby->game_data = game_data;
        lobby->delete_lobby_game_data = delete_lobby_game_data;
    }

}

// lobby constructor, it also initializes basic lobby data
Lobby *lobby_new (Server *server, unsigned int max_players) {

    Lobby *lobby = (Lobby *) malloc (sizeof (Lobby));
    if (lobby) {
        memset (lobby, 0, sizeof (Lobby));

        lobby->game_settings = NULL;

        GameServerData *game_data = (GameServerData *) server->serverData;
        // FIXME: we dont want to delete the players here!! they also have a refrence in the main avl tree!!
        lobby->players = avl_init (game_data->player_comparator, player_delete);
        lobby->owner = NULL; 
        
        lobby->max_players = max_players;
        lobby->players_fds = (struct pollfd *) calloc (max_players, sizeof (struct pollfd));
        lobby->compress_players = false;        // default

        lobby->game_data = NULL;
        lobby->delete_lobby_game_data = NULL;

        lobby->isRunning = false;
        lobby->inGame = false;
    }

    return lobby;

}

// deletes a lobby for ever -- called when we teardown the server
// we do not need to give any feedback to the players if there is any inside
void lobby_delete (void *ptr) {

    if (ptr) {
        Lobby *lobby = (Lobby *) ptr;

        lobby->inGame = false;          // just to be sure
        lobby->isRunning = false;
        lobby->owner = NULL;            // the owner is destroyed in the avl tree

        if (lobby->game_data) {
            if (lobby->delete_lobby_game_data) lobby->delete_lobby_game_data (lobby->game_data);
            else free (lobby->game_data);
        }

        if (lobby->players_fds) {
            memset (lobby->players_fds, 0, sizeof (struct pollfd) * lobby->max_players);
            free (lobby->players_fds);
        }

        if (lobby->players) {
            avl_clearTree (&lobby->players->root, lobby->players->destroy);
            free (lobby->players);
        }   

        if (lobby->game_settings) {
            if (lobby->delete_lobby_game_settings) lobby->delete_lobby_game_settings (lobby->game_settings);
            else free (lobby->game_settings);
        }

        free (lobby);
    }

}

// we remove any fd that was set to -1 for what ever reason
void compressPlayers (Lobby *lobby) {

    if (lobby) {
        lobby->compress_players = false;
        
        for (u16 i = 0; i < lobby->players_nfds; i++) {
            if (lobby->players_fds[i].fd == -1) {
                for (u16 j = i; j < lobby->players_nfds; j++)
                    lobby->players_fds[i].fd = lobby->players_fds[j + 1].fd;

                i--;
                lobby->players_nfds--;
            }
        }
    }

}

void handleGamePacket (void *);

// FIXME: create a similar logic to on hold players!!!
// recieves packages from players inside the lobby
void handlePlayersInLobby (void *data) {

    if (!data) {
        logMsg (stderr, ERROR, SERVER, "Can't handle packets of a NULL lobby!");
        return;
    }

    Server *server = ((ServerLobby *) data)->server;
    Lobby *lobby = ((ServerLobby *) data)->lobby;

    ssize_t rc;                                  // retval from recv -> size of buffer
    char packetBuffer[MAX_UDP_PACKET_SIZE];      // buffer for data recieved from fd
    GamePacketInfo *info = NULL;

    #ifdef CERVER_DEBUG
    logMsg (stdout, SUCCESS, SERVER, "New lobby has started!");
    #endif

    int poll_retval;    // ret val from poll function
    int currfds;        // copy of n active server poll fds
    while (lobby->isRunning) {
        poll_retval = poll (lobby->players_fds, lobby->players_nfds, lobby->pollTimeout);

        // poll failed
        if (poll_retval < 0) {
            logMsg (stderr, ERROR, SERVER, "Lobby poll failed!");
            perror ("Error");
            lobby->isRunning = false;
            break;
        }

        // if poll has timed out, just continue to the next loop... 
        if (poll_retval == 0) {
            #ifdef DEBUG
            logMsg (stdout, DEBUG_MSG, SERVER, "Lobby poll timeout.");
            #endif
            continue;
        }

        // one or more fd(s) are readable, need to determine which ones they are
        currfds = lobby->players_nfds;
        for (u8 i = 0; i < currfds; i++) {
            if (lobby->players_fds[i].fd <= -1) continue;

            if (lobby->players_fds[i].revents == 0) continue;

            if (lobby->players_fds[i].revents != POLLIN) 
                logMsg (stderr, ERROR, GAME, "Lobby poll - Unexpected poll result!");

            do {
                rc = recv (lobby->players_fds[i].fd, packetBuffer, sizeof (packetBuffer), 0);
                
                if (rc < 0) {
                    if (errno != EWOULDBLOCK) {
                        logMsg (stderr, ERROR, SERVER, "On hold recv failed!");
                        perror ("Error:");
                    }

                    break;  // there is no more data to handle
                }

                if (rc == 0) break;

                info = newGamePacketInfo (server, lobby, 
                    getPlayerBySock (lobby->players->root, lobby->players_fds[i].fd), packetBuffer, rc);

                thpool_add_work (server->thpool, (void *) handleGamePacket, info);
            } while (true);
        }

        if (lobby->compress_players) compressPlayers (lobby);
    }

}

void deleteGamePacketInfo (void *data);



// create a list to manage the server lobbys
// called when we init the game server
u8 game_initLobbys (GameServerData *gameData, u8 n_lobbys) {

    if (!gameData) {
        logMsg (stderr, ERROR, SERVER, "Can't init lobbys in a NULL game data!");
        return 1;
    }

    if (gameData->currentLobbys) 
        logMsg (stdout, WARNING, SERVER, "The server has already a list of lobbys.");
    else {
        gameData->currentLobbys = dlist_init (deleteLobby, NULL);
        if (!gameData->currentLobbys) {
            logMsg (stderr, ERROR, NO_TYPE, "Failed to init server's lobby list!");
            return 1;
        }
    }

    if (gameData->lobbyPool) 
        logMsg (stdout, WARNING, SERVER, "The server has already a pool of lobbys.");
    else {
        gameData->lobbyPool = pool_init (deleteLobby);
        if (!gameData->lobbyPool) {
            logMsg (stderr, ERROR, NO_TYPE, "Failed to init server's lobby pool!");
            return 1;
        }
    }
    
    for (u8 i = 0; i < n_lobbys; i++) pool_push (gameData->lobbyPool, malloc (sizeof (Lobby)));

    #ifdef DEBUG
    logMsg (stdout, DEBUG_MSG, GAME, "Lobbys have been init in the game server.");
    #endif

    return 0;

}

// loads the settings for the selected game type from the game server data
GameSettings *getGameSettings (Config *gameConfig, GameType gameType) {

    ConfigEntity *cfgEntity = config_get_entity_with_id (gameConfig, gameType);
	if (!cfgEntity) {
        logMsg (stderr, ERROR, GAME, "Problems with game settings config!");
        return NULL;
    } 

    GameSettings *settings = (GameSettings *) malloc (sizeof (GameSettings));

    char *playerTimeout = config_get_entity_value (cfgEntity, "playerTimeout");
    if (playerTimeout) {
        settings->playerTimeout = atoi (playerTimeout);
        free (playerTimeout);
    } 
    else {
        logMsg (stdout, WARNING, GAME, "No player timeout found in cfg. Using default.");        
        settings->playerTimeout = DEFAULT_PLAYER_TIMEOUT;
    }

    #ifdef DEBUG
    logMsg (stdout, DEBUG_MSG, GAME, createString ("Player timeout: %i", settings->playerTimeout));
    #endif

    char *fps = config_get_entity_value (cfgEntity, "fps");
    if (fps) {
        settings->fps = atoi (fps);
        free (fps);
    } 
    else {
        logMsg (stdout, WARNING, GAME, "No fps found in cfg. Using default.");        
        settings->fps = DEFAULT_FPS;
    }

    #ifdef DEBUG
    logMsg (stdout, DEBUG_MSG, GAME, createString ("FPS: %i", settings->fps));
    #endif

    char *minPlayers = config_get_entity_value (cfgEntity, "minPlayers");
    if (minPlayers) {
        settings->minPlayers = atoi (minPlayers);
        free (minPlayers);
    } 
    else {
        logMsg (stdout, WARNING, GAME, "No min players found in cfg. Using default.");        
        settings->minPlayers = DEFAULT_MIN_PLAYERS;
    }

    #ifdef DEBUG
    logMsg (stdout, DEBUG_MSG, GAME, createString ("Min players: %i", settings->minPlayers));
    #endif

    char *maxPlayers = config_get_entity_value (cfgEntity, "maxPlayers");
    if (maxPlayers) {
        settings->maxPlayers = atoi (maxPlayers);
        free (maxPlayers);
    } 
    else {
        logMsg (stdout, WARNING, GAME, "No max players found in cfg. Using default.");        
        settings->maxPlayers = DEFAULT_MIN_PLAYERS;
    }

    #ifdef DEBUG
    logMsg (stdout, DEBUG_MSG, GAME, createString ("Max players: %i", settings->maxPlayers));
    #endif

    settings->gameType = gameType;

    return settings;

}

// FIXME: lobby thread with a bool
// TODO: add a timestamp of the creation of the lobby
// creates a new lobby and inits his values with an owner and a game type
Lobby *createLobby (Server *server, Player *owner, GameType gameType) {

    if (!server) {
        logMsg (stderr, ERROR, SERVER, "Don't know in which server to create the lobby!");
        return NULL;
    }

    else {
        if (server->type != GAME_SERVER) {
            logMsg (stderr, ERROR, SERVER, "Can't create a lobby in a server of incorrect type!");
            return NULL;
        }
    }

    if (!owner) {
        logMsg (stderr, ERROR, SERVER, "A NULL player can't create a lobby!");
        return NULL;
    }

    #ifdef DEBUG
    logMsg (stdout, DEBUG_MSG, GAME, "Creatting a new lobby...");
    #endif

    GameServerData *data = (GameServerData *) server->serverData;
    Lobby *lobby = newLobby (server);

    lobby->owner = owner;
    lobby->settings = getGameSettings (data->gameSettingsConfig, gameType);
    if (!lobby->settings) {
        logMsg (stderr, ERROR, GAME, "Failed to get the settings for the new lobby!");
        lobby->owner = NULL;
        // send the lobby back to the object pool
        pool_push (data->lobbyPool, lobby);

        return NULL;
    } 

    // init the clients/players structures inside the lobby
    lobby->players = avl_init (player_comparator_client_id, deletePlayer);
    lobby->players_nfds = 0;
    lobby->compress_players = false;
    lobby->pollTimeout = server->pollTimeout;    // inherit the poll timeout from server

    lobby->deleteLobbyGameData = data->deleteLobbyGameData;

    if (!player_addToLobby (server, lobby, owner)) {
        lobby->inGame = false;
        lobby->isRunning = true;

        // add the lobby the server active ones
        dlist_insert_after (data->currentLobbys, dlist_end (data->currentLobbys), lobby);

        // create a unique thread to handle this lobby
        ServerLobby *sl = (ServerLobby *) malloc (sizeof (ServerLobby));
        sl->server = server;
        sl->lobby = lobby;

        // thpool_add_work (server->thpool, (void *) handlePlayersInLobby, sl);

        return lobby;
    }

    else {
        logMsg (stderr, ERROR, GAME, "Failed to register the owner of the lobby to the lobby!");
        logMsg (stderr, ERROR, GAME, "Failed to create new lobby!");

        pool_push (data->lobbyPool, lobby);      // dispose the lobby

        return NULL;
    } 

    return lobby;

}

// a lobby should only be destroyed when there are no players left or if we teardown the server
u8 destroyLobby (Server *server, Lobby *lobby) {

    if (server && lobby) {
        if (server->type == GAME_SERVER) {
            GameServerData *gameData = (GameServerData *) server->serverData;
            if (gameData) {
                // the game function must have a check for this every loop!
                if (lobby->inGame) lobby->inGame = false;
                if (lobby->gameData) {
                    if (lobby->deleteLobbyGameData) lobby->deleteLobbyGameData (lobby->gameData);
                    else free (lobby->gameData);
                }

                if (lobby->players_nfds > 0) {
                    // send the players the correct package so they can handle their own logic
                    // expected player behaivor -> leave the lobby 
                    size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
                    void *destroyPacket = generatePacket (GAME_PACKET, packetSize);
                    if (destroyPacket) {
                        char *end = destroyPacket + sizeof (PacketHeader);
                        RequestData *reqdata = (RequestData *) end;
                        reqdata->type = LOBBY_DESTROY;

                        broadcastToAllPlayers (lobby->players->root, server, destroyPacket, packetSize);
                        free (destroyPacket);
                    }

                    else 
                        logMsg (stderr, ERROR, PACKET, "Failed to create lobby destroy packet!");

                    // this should stop the lobby poll thread
                    lobby->isRunning = false;

                    // remove the players from this structure and send them to the server's players
                    Player *tempPlayer = NULL;
                    while (lobby->players_nfds > 0) {
                        tempPlayer = (Player *) lobby->players->root->id;
                        if (tempPlayer) player_removeFromLobby (server, lobby, tempPlayer);
                    }
                }

                lobby->owner = NULL;
                if (lobby->settings) free (lobby->settings);

                // we are safe to clear the lobby structure
                // first remove the lobby from the active ones, then send it to the inactive ones
                ListElement *le = dlist_get_element (gameData->currentLobbys, lobby);
                if (le) {
                    void *temp = dlist_remove_element (gameData->currentLobbys, le);
                    if (temp) pool_push (gameData->lobbyPool, temp);
                }

                else {
                    logMsg (stdout, WARNING, GAME, "A lobby wasn't found in the current lobby list.");
                    deleteLobby (lobby);   // destroy the lobby forever
                } 
                
                return 0;   // success
            }

            else logMsg (stderr, ERROR, SERVER, "No game data found in the server!");
        }
    }

    return 1;

}

// TODO: maybe the admin can add a custom ptr to a custom function?
// FIXME:
// TODO: pass the correct game type and maybe create a more advance algorithm
// finds a suitable lobby for the player
Lobby *findLobby (Server *server) {

    // FIXME: how do we want to handle these errors?
    // perform some check here...
    if (!server) return NULL;
    if (server->type != GAME_SERVER) {
        logMsg (stderr, ERROR, SERVER, "Can't search for lobbys in non game server.");
        return NULL;
    }
    
    GameServerData *gameData = (GameServerData *) server->serverData;
    if (!gameData) {
        logMsg (stderr, ERROR, SERVER, "NULL reference to game data in game server!");
        return NULL;
    }

    // 11/11/2018 -- we have a simple algorithm that only searches for the first available lobby
    Lobby *lobby = NULL;
    for (ListElement *e = dlist_start (gameData->currentLobbys); e != NULL; e = e->next) {
        lobby = (Lobby *) e->data;
        if (lobby) {
            if (!lobby->inGame) {
                if (lobby->players_nfds < lobby->settings->maxPlayers) {
                    // we have found a suitable lobby
                    break;
                }
            }
        }
    }

    // TODO: what do we do if we do not found a lobby? --> create a new one?
    if (!lobby) {

    }

    return lobby;

}

// called by a registered player that wants to join a lobby on progress
u8 joinLobby (Server *server, Lobby *lobby, Player *player) {

    if (!lobby || !player) return 1;

    // check if for whatever reason a player al ready inside the lobby wants to join...
    if (player_isInLobby (player, lobby)) {
        #ifdef DEBUG
        logMsg (stdout, DEBUG_MSG, GAME, "A player tries to join the same lobby he is in.");
        #endif
        // FIXME:
        // sendErrorPacket (server, player->client, ERR_JOIN_LOBBY, 
        //     "You can't join the same lobby you are in!");
        return 1;
    }

    // check that the player can join the actual game...
    if (lobby->inGame) {
        #ifdef DEBUG
        logMsg (stdout, DEBUG_MSG, GAME, "A player tried to join a lobby that is in game.");
        #endif
        // FIXME:
        // sendErrorPacket (server, player->client, ERR_JOIN_LOBBY, "A game is in progress in the lobby!");
        return 1;
    }

    // lobby is already full
    if (lobby->players_nfds >= lobby->settings->maxPlayers) {
        #ifdef DEBUG
        logMsg (stdout, DEBUG_MSG, GAME, "A player tried to join a full lobby.");
        #endif
        // FIXME:
        // sendErrorPacket (server, player->client, ERR_JOIN_LOBBY, "The lobby is already full!");
        return 1;
    }

    if (!player_addToLobby (server, lobby, player)) {
        // sync the in lobby player(s) and the new player
        sendLobbyPacket (server, lobby);

        return 0;   // success
    }
    
    else {
        logMsg (stderr, ERROR, PLAYER, "Failed to register a player to a lobby!");
        return 1;
    }

}

// FIXME: client socket!!
// FIXME: send packet!
// TODO: add a timestamp when the player leaves
// called when a player requests to leave the lobby
u8 leaveLobby (Server *server, Lobby *lobby, Player *player) {

    if (!lobby || player) return 1;

    // make sure that the player is inside the lobby
    if (player_isInLobby (player, lobby)) {
        // check to see if the player is the owner of the lobby
        bool wasOwner = false;
        // TODO: we should be checking for the player's id instead
        // if (lobby->owner->client->clientSock == player->client->clientSock) 
        //     wasOwner = true;

        if (player_removeFromLobby (server, lobby, player)) return 1;

        // there are still players in the lobby
        if (lobby->players_nfds > 0) {
            if (lobby->inGame) {
                // broadcast the other players that the player left
                // we need to send an update lobby packet and the players handle the logic
                sendLobbyPacket (server, lobby);

                // if he was the owner -> set a new owner of the lobby -> first one in the array
                if (wasOwner) {
                    Player *temp = NULL;
                    u8 i = 0;
                    do {
                        temp = getPlayerBySock (lobby->players->root, lobby->players_fds[i].fd);
                        if (temp) {
                            lobby->owner = temp;
                            size_t packetSize = sizeof (PacketHeader) + sizeof (RequestData);
                            void *packet = generatePacket (GAME_PACKET, packetSize);
                            if (packet) {
                                // server->protocol == IPPROTO_TCP ?
                                // tcp_sendPacket (temp->client->clientSock, packet, packetSize, 0) :
                                // udp_sendPacket (server, packet, packetSize, temp->client->address);
                                free (packet);
                            }
                        }

                        // we got a NULL player in the structures -> we don't expect this to happen!
                        else {
                            logMsg (stdout, WARNING, GAME, 
                                "Got a NULL player when searching for new owner!");
                            lobby->players_fds[i].fd = -1;
                            lobby->compress_players = true; 
                            i++;
                        }
                    } while (!temp);
                }
            }

            // players are in the lobby screen -> owner destroyed the lobby
            else destroyLobby (server, lobby);
        }

        // player that left was the last one 
        // 21/11/2018 -- destroy lobby is in charge of correctly ending the game
        else destroyLobby (server, lobby);
        
        return 0;   // the player left the lobby successfully
    }

    else {
        #ifdef DEBUG
            logMsg (stderr, ERROR, GAME, "The player doesn't belong to the lobby!");
        #endif
    }

    return 1;

}