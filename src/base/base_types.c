#include <stdio.h>
#include "base.h"

#define BUFFER_EXPAND 200

List_t *List_build(char *file, int line, void *x, ...) {
    va_list ap;
    List_t *list;
    List_t **p = &list;
    va_start(ap, x);
    for ( ; x; x = va_arg(ap, void*)) {
        NEWHOP(*p, file, line);
        (*p)->payload = x;
        p = &(*p)->next;
    }
    *p = NULL;
    va_end(ap);
    return list;
}

List_t *List_add(char *filename, int line, List_t *list, void *payload, int tag) {
    if (!list) {
        List_t *l = List_build(filename, line, payload, (void*)NULL);
        l->tag = tag;
        return l;
    }
    List_t **p = &list;
    while (*p)
        p = &(*p)->next;
    NEW(*p);
    (*p)->payload = payload;
    (*p)->next = NULL;
    (*p)->tag = tag;
    return list;
}

List_t *List_drop(List_t *list, void *x) {
    // Get to an element that is not the payload
    if (list) {
        while (list && list->payload == x) {
            List_t *lp = list;
            list = list->next;
            FREE(lp);
        }
        List_t *lp = list;
        while (lp && lp->next) {
            if (lp->next->payload == x) {
                List_t *tmp = lp->next;
                lp->next = lp->next->next;
                FREE(tmp);
            }
            lp = lp->next;

        }
    }
    return list;
}

List_t *List_popLeft(List_t **list) {
    assert(list && *list);
    if (!*list) {
        return 0;
    } else if (!(*list)->next) {
        List_t *l = *list;
        *list = 0;
        return l;
    } else {
        List_t *l = *list;
        *list = (*list)->next;
        return l;
    }
}

List_t *List_popRight(List_t **list) {
    assert(list && *list);
    if (!*list) {
        return 0;
    }
    List_t *lt = *list;
    while (lt->next) {
        if (!lt->next->next) {
            List_t *l = lt->next;
            lt->next = 0;
            return l;
        }
        lt = lt->next;
    }
    List_t *l = *list;
    *list = 0;
    return l;
}

List_t *List_new(void *payload, int tag) {
    List_t *list;
    NEW(list);
    list->payload = payload;
    list->tag = tag;
    list->next = NULL;
    return list;
}

void List_dispose(List_t **list) {
    if (list && *list) {
        // assert(list && *list);
        if ((*list)->next) {
            List_dispose(&(*list)->next);
        }
        if ((*list)->payload) FREE((*list)->payload);
        FREE(*list);
        *list = NULL;
    }
}

void List_orphan(List_t **list) {
    assert(list && *list);
    if ((*list)->next) {
        List_orphan(&(*list)->next);
    }
    (*list)->payload = NULL;
    FREE(*list);
    *list = NULL;
}

int List_size(List_t *list) {
    int items = 0;
    for (List_t *lp = list; lp; lp = lp->next) {
        if (lp) items++;
    }
    return items;
}

Buffer_t *Buffer_new(size_t bytes_per_item, size_t capacity) {
    Buffer_t *buffer;
    NEW(buffer);
    buffer->buffer = Mem_alloc(bytes_per_item*capacity, __FILE__, __LINE__);
    if (!buffer->buffer) {
        FREE(buffer);
        return NULL;
    }
    buffer->capacity = capacity;
    buffer->stored = 0;
    buffer->bytes_per_item = bytes_per_item;
    return buffer;
}

int Buffer_expand(Buffer_t *buffer, size_t n_items) {
    char *index;
    NEWN(index, (buffer->capacity+n_items)*buffer->bytes_per_item);
    memcpy(index, buffer->buffer, buffer->stored*buffer->bytes_per_item);
    FREE(buffer->buffer);
    buffer->buffer = index;
    // if(RESIZE(buffer->buffer, buffer->bytes_per_item*buffer->capacity + n_items*buffer->bytes_per_item)) {
    //     return 0;
    // };
    buffer->capacity += n_items;
    return 1;
}

int Buffer_write(Buffer_t *buffer, size_t n_items, const void *data) {
    size_t to_expand = 0;
    while (buffer->stored + n_items > buffer->capacity + to_expand) {
        to_expand += BUFFER_EXPAND;
    }
    if (to_expand) {
        if (!Buffer_expand(buffer, to_expand)) {
            return -1;
        }
    }
    memcpy((char*)buffer->buffer + buffer->bytes_per_item*buffer->stored, data, buffer->bytes_per_item*n_items);
    int outIndex = buffer->stored;
    buffer->stored += n_items;
    return outIndex;
}

void Buffer_dispose(Buffer_t **buffer) {
    assert(buffer && *buffer);
    FREE((*buffer)->buffer);
    FREE(*buffer);
    *buffer = NULL;
}

void *Buffer_orphan(Buffer_t **buffer) {
    assert(buffer && *buffer);
    if (buffer && *buffer) {
        void *ptr = (*buffer)->buffer;
        FREE(*buffer);
        *buffer = NULL;
        return ptr;
    }
    return NULL;
}

List_t *Buffer_pluck(Buffer_t *buffer, int items, int *indexes) {
    List_t *list = 0;
    for (int i = 0; i < items; i++) {
        char* data = (char*)malloc(buffer->bytes_per_item);
        memcpy(data, ((char*)buffer->buffer) + buffer->bytes_per_item * indexes[i], buffer->bytes_per_item);
        list = List_append(list, data, indexes[i]);
    }
    return list;
}

int Buffer_consume(Buffer_t *buffer, Buffer_t **other, int destroyOther) {
    size_t reindex = buffer->stored;
    if (other && *other) {
        assert(buffer->bytes_per_item == (*other)->bytes_per_item);
        Buffer_write(buffer, (*other)->stored, (*other)->buffer);
        // for (int i = 0; i < (*other)->stored; i++) {
        //     Buffer_write(buffer, (*other)->stored, (*other)->buffer);
        // }
        if (destroyOther) {
            FREE((*other)->buffer);
            FREE(*other);
            *other = NULL;
        }
    }
    return (int)reindex;
}

int Buffer_index(Buffer_t *buffer, void *data) {
    for (int i = 0; i < buffer->stored; i++) {
        if (memcmp((char*)buffer->buffer + (buffer->bytes_per_item*i), (char*)data, buffer->bytes_per_item) == 0) {
            return i;
        }
    }
    return -1;
}

void *Buffer_get(Buffer_t *buffer, ItemID item) {
    return (void*)(((char*)buffer->buffer) + (buffer->bytes_per_item * item));
}

IDRegistry *Registry_new(int startSize, int growBy) {
    IDRegistry *registry;
    NEW(registry);
    registry->maxItems = startSize;
    registry->growBy = growBy;
    registry->numItems = 0;
    NEWN(registry->items, startSize);
    for (int i = 0; i < registry->maxItems; i++) registry->items[i] = 0;
    return registry;
}

ItemID Registry_register(IDRegistry *registry, void *item) {
    // NOTE: ELEMENT 0 IS ALWAYS 0 TO LEVERAGE NULL LOOKUPS.
    // THE VARIOUS +1 EXPRESSIONS IN HERE ARE A CONSEQUENCE OF THAT
    if (registry->numItems+1 >= registry->maxItems) {

        // Look for empty spot
        for (int i = 1; i < registry->maxItems; i++) {
            if (registry->items[i] == 0) {
                ItemID id = i;
                registry->items[i] = item;
                return id;
            }
        }

        // Failing that, expand the array
        void **ptr = registry->items;
        int additional = registry->growBy;
        while (registry->numItems+1 >= registry->maxItems + additional) additional += registry->growBy;

        void **newptr;
        NEWN(newptr, (registry->maxItems + additional));
        // NEWN(registry->items, registry->maxItems + additional);
        for (int i = 0; i < registry->numItems+1; i++) {
             newptr[i] = ptr[i];
        }
        for (int i = 0; i < additional; i++) {
            newptr[registry->numItems+1+i] = 0;
        }
        registry->maxItems += additional;
        FREE(ptr);
        registry->items = newptr;
        ptr = 0;
        // return itemID;
    }
    // ItemID id = registry->numItems+1;
    ItemID id = registry->numItems+1;
    registry->items[id] = item;
    registry->numItems += 1;
    return id;
}

void *Registry_get(IDRegistry *registry, ItemID item) {
    return registry->items[item];
}

void Registry_remove(IDRegistry *registry, ItemID item, int destroy) {
    if (destroy) FREE(registry->items[item]);
    registry->items[item] = 0;
}

void Registry_reregister(IDRegistry *registry, ItemID itemID, void *item) {
    registry->items[itemID] = item;
}

void Registry_dispose(IDRegistry **registry) {
    for (int i = 0; i < (*registry)->maxItems; i++) {
        if ((*registry)->items[i]) {
            FREE((*registry)->items[i]);
        }
    }
    FREE(*registry);
    *registry = 0;
}

struct HashMap_t {
    int intMap;
    List_t *map[1024];
};

Mapping_t *Mapping_new(const char *key, void *value) {
    Mapping_t *mapping;
    NEW(mapping);
    mapping->key = copyString(key);
    mapping->value = value;
    return mapping;
}

HashMap_t *HashMap_new_hop(char *file, int line) {
    HashMap_t *map;
    NEWHOP(map, file, line);
    map->intMap = 0;
    for (int i = 0; i < 1024; i++) {
        map->map[i] = 0;
    }
    return map;
}

HashMap_t *HashMap_newInt_hop(char *file, int line) {
    HashMap_t *map;
    NEWHOP(map, file, line);
    map->intMap = 1;
    for (int i = 0; i < 1024; i++) {
        map->map[i] = 0;
    }
    return map;
}

size_t HashMap_hash(const char *key) {
    size_t hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void HashMap_setInt(HashMap_t *map, const char *key, int payload) {
    assert(map->intMap);
    int hash = (int)(HashMap_hash(key) % 1024);
    for (List_t *l = map->map[hash]; l; l = l->next) {
        char *testKey = l->payload;
        if (strcmp(testKey, key) == 0) {
            l->tag = payload;
            return;
        }
    }
    map->map[hash] = List_append(map->map[hash], copyString(key), payload);
}

void HashMap_set(HashMap_t *map, const char *key, void *payload) {
    assert(!map->intMap);
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

void *HashMap_pop(HashMap_t *map, const char *key) {
    assert(!map->intMap);
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

int HashMap_popInt(HashMap_t *map, const char *key) {
    assert(map->intMap);
    int hash = (int)(HashMap_hash(key) % 1024);
    List_t **l = &map->map[hash];
    while (*l) {
        char *testKey = (*l)->payload;
        if (strcmp(testKey, key) == 0) {
            int payload = (*l)->tag;
            *l = (*l)->next;
            return payload;
        }
    }
    return -1;
}

int HashMap_getInt(HashMap_t *map, const char *key) {
    assert(map->intMap);
    int hash = (int)(HashMap_hash(key) % 1024);
    for (List_t *l = map->map[hash]; l; l = l->next) {
        char *testKey = l->payload;
        if (strcmp(key, testKey) == 0) {
            return l->tag;
        }
    }
    return -1;
}

void *HashMap_get(HashMap_t *map, const char *key) {
    assert(!map->intMap);
    int hash = (int)(HashMap_hash(key) % 1024);
    for (List_t *l = map->map[hash]; l; l = l->next) {
        Mapping_t *mapping = l->payload;
        if (strcmp(key, mapping->key) == 0) {
            return mapping->value;
        }
    }
    return 0;
}

void HashMap_dispose(HashMap_t **map) {
    HashMap_t *h = *map;
    for (int i = 0; i < 1024; i++) {
        if (h->map[i])
            List_dispose(&h->map[i]);
    }
    FREE(*map);
    *map = 0;
}

// typedef struct StringDescriptor {
//     size_t numChars;
//     size_t spaceReserved;
//     char *characters;
// } StringDescriptor;

// StringDescriptor *StringDescriptor_new(size_t space) {
//     StringDescriptor *d;
//     NEW(d);
//     d->numChars = 0;
//     d->spaceReserved = space;
//     NEWN(d->characters, space);
//     return d;
// }

// size_t String_size(String string) {
//     return *((size_t*)string)-2;
// }
// String String_consume(String *string, String other);
// String String_copy(const String string);
// String String_new(const char *strIn);
// void String_dump(const String string);