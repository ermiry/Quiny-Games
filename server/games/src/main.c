#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <signal.h>

#include "cerver.h"

#include "utils/log.h"

Server *main_server = NULL;

void close_program (int dummy);

static int init (void) {

    signal (SIGINT, close_program);
    // TODO: load app specific data

    return 0;

}

// TODO: destroy any app specific data
static int end () { return 0; }

// correctly closes any on-going server and exits
void close_program (int dummy) {
    
    cerver_teardown (main_server);
    end ();
    exit (0);

}

int main (void) {

    if (init ()) {
        logMsg (stderr, ERROR, NO_TYPE, "Failed to init app!");
        return EXIT_FAILURE;
    }

    main_server = cerver_createServer (NULL, GAME_SERVER, "Quiny Server");
    if (main_server) {
        // set our own functions to authenticate our clients
        // cerver_set_auth_method (gameServer, blackrock_authMethod);

        // // set our own functions to load and delete global game data
        // gs_set_loadGameData (gameServer, blackrock_loadGameData);
        // gs_set_deleteGameData (gameServer, blackrock_deleteGameData);

        // // set blackrock arcade game init function
        // gs_add_gameInit (gameServer, ARCADE, blackrock_init_arcade);

        // // set lobby game data destroy function, to delete our world and
        // // any other data that blackrock lobby uses
        // gs_set_lobbyDeleteGameData (gameServer, deleteBrGameData);

        // if (cerver_startServer (gameServer)) 
        //     logMsg (stderr, ERROR, SERVER, "Failed to start server!");
    } 

    else logMsg (stderr, ERROR, NO_TYPE, "Failed to create Quiny Server!");

    return end ();

}