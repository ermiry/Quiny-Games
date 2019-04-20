#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cerver/game/game.h"
#include "cerver/game/player.h"

#include "utils/myUtils.h"
#include "utils/log.h"

// TODO: better id handling and management
u16 nextPlayerId = 0;

void player_set_delete_player_data (Player *player, Action destroy) {

    if (player) player->destroy_player_data = destroy;

}

// TODO: maybe later the id can be how we store it inside our databse
// FIXME: player ids, we cannot have infinite ids!!
// constructor for a new player
Player *player_new (Client *client, const char *session_id, void *player_data) {

    Player *new_player = (Player *) malloc (sizeof (Player));

    new_player->client = client;
    if (session_id) {
        new_player->session_id = (char *) calloc (strlen (session_id) + 1, sizeof (char));
        strcpy ((char *) new_player->session_id, session_id);
    }

    else new_player->session_id = NULL;

    new_player->alive = false;
    new_player->inLobby = false;

    new_player->components = NULL;
    new_player->data = player_data;

    new_player->id = nextPlayerId;
    nextPlayerId++;

    return new_player;

}

// FIXME: correctly destroy the game components
// TODO: what happens with the client data??
// deletes a player struct for ever
void player_delete (void *data) {

    if (data) {
        Player *player = (Player *) data;
        if (player->destroy_player_data) player->destroy_player_data (player->data);
        else if (player->data) free (player->data);

        free (player);
    }

}

// comparator for players's avl tree
int player_comparator_client_id (const void *a, const void *b) {

    if (a && b) {
        Player *player_a = (Player *) a;
        Player *player_b = (Player *) b;

        return strcmp (player_a->client->clientID, player_b->client->clientID);
    }

}

// inits the players server's structures
u8 players_init (GameServerData *gameData, u8 n_players) {

    if (!gameData) {
        logMsg (stderr, ERROR, SERVER, "Can't init players in a NULL game data!");
        return 1;
    }

    if (gameData->players)
        logMsg (stdout, WARNING, SERVER, "The server already has an avl of players!");
    else {
        gameData->players = avl_init (player_comparator_client_id, player_delete);
        if (!gameData->players) {
            logMsg (stderr, ERROR, NO_TYPE, "Failed to init server's players avl!");
            return 1;
        }
    } 

    if (gameData->playersPool)  
        logMsg (stdout, WARNING, SERVER, "The server already has a pool of players.");
    else {
        gameData->playersPool = pool_init (player_delete);
        if (!gameData->playersPool) {
            logMsg (stderr, ERROR, NO_TYPE, "Failed to init server's players pool!");
            return 1;
        }
    } 

    for (u8 i = 0; i < n_players; i++) pool_push (gameData->playersPool, malloc (sizeof (Player)));

    #ifdef DEBUG
        logMsg (stdout, DEBUG_MSG, GAME, "Players have been init in the game server.");
    #endif

    return 0;

}

// adds a player to the game server data main structures
void player_register_to_server (Server *server, Player *player) {

    if (server && player) {
        if (server->type == GAME_SERVER) {
            GameServerData *gameData = (GameServerData *) server->serverData;
            if (gameData->players) avl_insertNode (gameData->players, player);

            #ifdef CERVER_DEBUG
                logMsg (stdout, DEBUG_MSG, GAME, "Registered a player to the server.");
            #endif
        }

        else {
            #ifdef DEBUG
                logMsg (stdout, WARNING, SERVER, 
                    "Trying to add a player to a server of incompatible type!");
            #endif
        }
    }

}

// FIXME: client socket!!
// removes a player from the server's players struct (avl) and also removes the player
// client from the main server poll
void player_unregister_to_server (Server *server, Player *player) {

    // if (server && player) {
    //     if (server->type == GAME_SERVER) {
    //         GameServerData *gameData = (GameServerData *) server->serverData;
    //         if (gameData) {
    //             // remove the player client from the main server poll
    //             Client *c; // = getClientBySock (server->clients, player->client->clientSock);
    //             if (c) client_unregisterFromServer (server, player->client);

    //             // remove the player from the servers players
    //             avl_removeNode (gameData->players, player);
    //         }
    //     }
    // }

}

// FIXME: client socket!!
// make sure that the player is inside the lobby
bool player_is_in_lobby (Player *player, Lobby *lobby) {

    /* if (player && lobby) {
        for (u8 i = 0; i < lobby->players_nfds; i++) 
            if (lobby->players_fds[i].fd == player->client->clientSock)
                return true;

    } */

    return false;

}

// add a player to the lobby structures
u8 player_add_to_lobby (Server *server, Lobby *lobby, Player *player) {

    if (lobby && player) {
        if (server->type == GAME_SERVER) {
            GameServerData *gameData = (GameServerData *) server->serverData;
            if (gameData) {
                if (!player_isInLobby (player, lobby)) {
                    Player *p = avl_removeNode (gameData->players, player);
                    if (p) {
                        i32 client_sock_fd = player->client->active_connections[0];

                        avl_insertNode (lobby->players, p);

                        // for (u32 i = 0; i < poll_n_fds; i++) {
                        //     if (server->fds[i].fd == client_sock_fd) {
                        //         server->fds[i].fd = -1;
                        //         server->fds[i].events = -1;
                        //     }
                        // } 

                        lobby->players_fds[lobby->players_nfds].fd = client_sock_fd;
                        lobby->players_fds[lobby->players_nfds].events = POLLIN;
                        lobby->players_nfds++;

                        player->inLobby = true;

                        return 0;
                    }

                    else logMsg (stderr, ERROR, GAME, "Failed to get player from avl!");
                }
            }
        }
    }

    return 1;    

}

// FIXME: client socket!!
// removes a player from the lobby's players structures and sends it to the game server's players
u8 player_remove_from_lobby (Server *server, Lobby *lobby, Player *player) {

    if (server && lobby && player) {
        if (server->type == GAME_SERVER) {
            GameServerData *gameData = (GameServerData *) gameData;
            if (gameData) {
                // make sure that the player is inside the lobby...
                if (player_isInLobby (player, lobby)) {
                    // create a new player and add it to the server's players
                    // Player *p = newPlayer (gameData->playersPool, NULL, player);

                    // remove from the poll fds
                    for (u8 i = 0; i < lobby->players_nfds; i++) {
                        /* if (lobby->players_fds[i].fd == player->client->clientSock) {
                            lobby->players_fds[i].fd = -1;
                            lobby->players_nfds--;
                            lobby->compress_players = true;
                            break;
                        } */
                    }

                    // delete the player from the lobby
                    avl_removeNode (lobby->players, player);

                    // p->inLobby = false;

                    // player_registerToServer (server, p);

                    return 0;
                }
            }
        }
    }

    return 1;

}

// recursively get the player associated with the socket
Player *player_get_by_socket (AVLNode *node, i32 socket_fd) {

    if (node) {
        Player *player = NULL;

        player = getPlayerBySock (node->right, socket_fd);

        if (!player) {
            if (node->id) {
                player = (Player *) node->id;
                
                // search the socket fd in the clients active connections
                for (int i = 0; i < player->client->n_active_cons; i++)
                    if (socket_fd == player->client->active_connections[i])
                        return player;

            }
        }

        if (!player) player = getPlayerBySock (node->left, socket_fd);

        return player;
    }

    return NULL;

}

// FIXME: client socket!!
// broadcast a packet/msg to all clients/players inside an avl structure
void player_broadcast_to_all (AVLNode *node, Server *server, void *packet, size_t packetSize) {

    if (node && server && packet && (packetSize > 0)) {
        broadcastToAllPlayers (node->right, server, packet, packetSize);

        // send packet to curent player
        if (node->id) {
            Player *player = (Player *) node->id;
            if (player) 
                server_sendPacket (server, player->client->active_connections[0],
                    player->client->address, packet, packetSize);
        }

        broadcastToAllPlayers (node->left, server, packet, packetSize);
    }

}

// performs an action on every player in an avl tree 
void player_travers (AVLNode *node, Action action, void *data) {

    if (node && action) {
        player_travers (node->right, action, data);

        if (node->id) {
            PlayerAndData pd = { .playerData = node->id, .data = data };
            action (&pd);
        } 

        player_travers (node->left, action, data);
    }

}

// TODO:
// this is used to clean disconnected players inside a lobby
// if we haven't recieved any kind of input from a player, disconnect it 
void checkPlayerTimeouts (void) {}

// TODO: 10/11/2018 -- do we need this?
// when a client creates a lobby or joins one, it becomes a player in that lobby
void tcpAddPlayer (Server *server) {}

// TODO: 10/11/2018 -- do we need this?
// TODO: as of 22/10/2018 -- we only have support for a tcp connection
// when we recieve a packet from a player that is not in the lobby, we add it to the game session
void udpAddPlayer () {}

// TODO: 10/11/2018 -- do we need this?
// TODO: this is used in space shooters to add a new player using a udp protocol
// FIXME: handle a limit of players!!
void addPlayer (struct sockaddr_storage address) {

    // TODO: handle ipv6 ips
    /* char addrStr[IP_TO_STR_LEN];
    sock_ip_to_string ((struct sockaddr *) &address, addrStr, sizeof (addrStr));
    logMsg (stdout, SERVER, PLAYER, createString ("New player connected from ip: %s @ port: %d.\n", 
        addrStr, sock_ip_port ((struct sockaddr *) &address))); */

    // TODO: init other necessarry game values
    // add the new player to the game
    // Player newPlayer;
    // newPlayer.id = nextPlayerId;
    // newPlayer.address = address;

    // vector_push (&players, &newPlayer);

    // FIXME: this is temporary
    // spawnPlayer (&newPlayer);

}