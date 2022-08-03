#ifndef hashmap_H
#define hashmap_H

#include "list.h"

typedef struct HashMap_t HashMap_t;
HashMap_t *HashMap_new();
void HashMap_set(HashMap_t *map, char *key, void *payload);
void *HashMap_pop(HashMap_t *map, char *key);
void *HashMap_get(HashMap_t *map, char *key);

#endif