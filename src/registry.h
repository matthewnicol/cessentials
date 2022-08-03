#ifndef registry_H
#define registry_H

typedef int ItemID;

typedef struct IDRegistry {
    int numItems;
    int maxItems;
    int growBy;
    void **items;
} IDRegistry;

IDRegistry *Registry_new(int startSize, int growBy);
ItemID Registry_register(IDRegistry *registry, void *item);
void Registry_reregister(IDRegistry *registry, ItemID itemID, void *item);
void *Registry_get(IDRegistry *registry, ItemID item);
void Registry_remove(IDRegistry *registry, ItemID item, int destroy);

#define REGISTRY_LOOKUP(A, B, C) A = Registry_get(B, C);

/**
 * I know - seems gross - but this bundles forgettable & tedious things
 * - Skip holes in registry due to freed items; don't take null as 'stop'
 * - Compensate for sentinal value (0): add 1 to numItems, and skip 0
 * - Manage pointer, including saving value on break.
 */

#define REGISTRY_FOREACH_I(A, B, N) A; \
            for (int N = 1; N < B->numItems+1; N++) \
                if ((A = B->items[N])) \

#endif