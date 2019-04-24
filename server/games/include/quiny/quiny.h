#ifndef _QUINY_H_
#define _QUINY_H_

#include <mongoc/mongoc.h>
#include <bson/bson.h>

#include "types/myString.h"

// quiny error codes
#define SERVER_ERROR                100
#define NO_ERRORS                   0
#define NOT_USER_FOUND              1
#define WRONG_PASSWORD              2
#define USER_NOT_FOUND              3

extern const char *uri_string;
extern const char *db_name;

typedef struct User {

    bson_oid_t oid;

    String *amazon_access_token;

    String *name;
    String *username;
    String *email;
    String *password;

} User;

// search for a user with the given username
// if we find one, check if the password match
extern User *quiny_user_get (const char *username, const char *password, int *errors);

// init quiny data & processes
extern int quiny_init (void);
// clean up quiny data
extern int quiny_end (void);

extern void quiny_handle_recieved_buffer (void *rcvd_buffer_data);

#endif