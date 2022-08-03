#include "registry.h"
#include "core/structures/control/mem.h"

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
        while (registry->numItems > registry->maxItems + additional) additional += registry->growBy;
        NEWN(registry->items, registry->maxItems + additional);
        registry->maxItems += additional;
        for (int i = 0; i < registry->maxItems; i++) registry->items[i] = ptr[i];
        for (int i = registry->maxItems; i < registry->maxItems + additional; i++) registry->items[i] = 0;
        registry->maxItems += additional;
        FREE(ptr);
        ptr = 0;
    }
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