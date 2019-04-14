#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quiny/quiny.h"

#include "mongo/mongo.h"

#include "types/myString.h"

static User *user_new (void) {

    User *user = (User *) malloc (sizeof (User));
    if (user) {
        memset (user, 0, sizeof (User));
        user->email = user->name = user->password = user->username = NULL;
    }

    return user;

}

static void user_delete (User *user) {

    if (user) {
        str_delete (user->email);
        str_delete (user->name);
        str_delete (user->password);
        str_delete (user->username);

        free (user);
    }

}