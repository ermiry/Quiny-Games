#ifndef _QUINY_H_
#define _QUINY_H_

#include <mongoc/mongoc.h>
#include <bson/bson.h>

typedef struct User {

    char *name;
    char *username;
    char *email;
    char *password;

} User;

#endif