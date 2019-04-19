#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <signal.h>

#include "cerver/cerver.h"

#include "quiny/quiny.h"

#include "utils/log.h"

Server *main_server = NULL;

void close_program (int dummy);

static int init (void) {

    signal (SIGINT, close_program);
    signal (SIGSEGV, close_program);
    signal (SIGABRT, close_program);
    return quiny_init ();

}

static int end () { return quiny_end (); }

// correctly closes any on-going server and exits
void close_program (int dummy) {
    
    cerver_teardown (main_server);
    exit (end ());

}

int main (void) {

    if (init ()) {
        logMsg (stderr, ERROR, NO_TYPE, "Failed to init app!");
        return EXIT_FAILURE;
    }

    // main_server = cerver_createServer (NULL, GAME_SERVER, "Quiny Server");
    // if (main_server) {
    //     // set our own functions to authenticate our clients
    //     // cerver_set_auth_method (gameServer, blackrock_authMethod);

    //     // // set our own functions to load and delete global game data
    //     // gs_set_loadGameData (gameServer, blackrock_loadGameData);
    //     // gs_set_deleteGameData (gameServer, blackrock_deleteGameData);

    //     // // set blackrock arcade game init function
    //     // gs_add_gameInit (gameServer, ARCADE, blackrock_init_arcade);

    //     // // set lobby game data destroy function, to delete our world and
    //     // // any other data that blackrock lobby uses
    //     // gs_set_lobbyDeleteGameData (gameServer, deleteBrGameData);

    //     // if (cerver_startServer (gameServer)) 
    //     //     logMsg (stderr, ERROR, SERVER, "Failed to start server!");
    // } 

    // else logMsg (stderr, ERROR, NO_TYPE, "Failed to create Quiny Server!");

    return end ();

}