#ifndef _QUINY_H_
#define _QUINY_H_

#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include "types/myString.h"

typedef struct User {

    String *name;
    String *username;
    String *email;
    String *password;

} User;

#endif