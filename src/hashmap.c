#include "hashmap.h"
#include "core/structures/control/mem.h"

typedef struct Mapping_t {
    char *key;
    void *value;
} Mapping_t;

struct HashMap_t {
    List_t *map[1024];
};

Mapping_t *Mapping_new(char *key, void *value) {
    Mapping_t *mapping;
    NEW(mapping);
    mapping->key = key;
    mapping->value = value;
    return mapping;
}

HashMap_t *HashMap_new() {
    HashMap_t *map;
    NEW(map);
    for (int i = 0; i < 1024; i++) {
        map->map[i] = 0;
    }
    return map;
}

size_t HashMap_hash(char *key) {
    size_t hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void HashMap_set(HashMap_t *map, char *key, void *payload) {
    int hash = (int)(HashMap_hash(key) % 1024);
    for (List_t *l = map->map[hash]; l; l = l->next) {
        Mapping_t *mapping = (Mapping_t*)l->payload;
        if (strcmp(mapping->key, key) == 0) {
            mapping->value = payload;
            return;
        }
    }
    map->map[hash] = List_append(map->map[hash], Mapping_new(key, payload), 0);
}

void *HashMap_pop(HashMap_t *map, char *key) {
    int hash = (int)(HashMap_hash(key) % 1024);
    List_t **l = &map->map[hash];
    while (*l) {
        Mapping_t *mapping = (*l)->payload;
        if (strcmp(mapping->key, key) == 0) {
            void *payload = mapping->value;
            *l = (*l)->next;
            return payload;
        }
    }
    return 0;
}

void *HashMap_get(HashMap_t *map, char *key) {
    int hash = (int)(HashMap_hash(key) % 1024);
    for (List_t *l = map->map[hash]; l; l = l->next) {
        Mapping_t *mapping = l->payload;
        if (strcmp(key, mapping->key) == 0) {
            return mapping->value;
        }
    }
    return 0;
}