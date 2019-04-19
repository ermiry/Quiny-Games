#ifndef _QUERY_PARSER_H_
#define _QUERY_PARSER_H_

#include "collections/dllist.h"

typedef struct KeyValuePair { char *key, *value; } KeyValuePair;

extern DoubleList *parse_query_into_pairs (const char *first, const char *last);
extern char *strip_path_from_query (char *str);

#endif