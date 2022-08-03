#include "core/structures/list.h"

#include "core/structures/control/mem.h"
#include "core/structures/control/assert.h"

List_t *List_list(void *x, ...) {
    va_list ap;
    List_t *list;
    List_t **p = &list;
    va_start(ap, x);
    for ( ; x; x = va_arg(ap, void*)) {
        NEW(*p);
        (*p)->payload = x;
        p = &(*p)->next;
    }
    *p = NULL;
    va_end(ap);
    return list;
}

List_t *List_taggedList(int tag, void *x, ...) {
    va_list ap;
    List_t *list;
    List_t **p = &list;
    va_start(ap, x);
    for ( ; x; tag = va_arg(ap, int), x = va_arg(ap, void*)) {
        NEW(*p);
        (*p)->payload = x;
        (*p)->tag = tag;
        p = &(*p)->next;
    }
    *p = NULL;
    va_end(ap);
    return list;
}

List_t *List_append(List_t *list, void *payload, int tag) {
    if (!list) {
        List_t *l = List_list(payload, (void*)NULL);
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

List_t *List_copy(List_t *list) {
    List_t *head, **p = &head;
    for ( ; list ; list = list->next) {
        NEW(*p);
        (*p)->payload = list->payload;
        p = &(*p)->next;
    }
    *p = NULL;
    return head;
}

int filterCompare(void *payload, int tag, void **customData) {
    return strcmp(payload, *customData) == 0;
}

// List_t *List_partition(List_t **list, int (*filter)(void *payload, int tag, void **customData), void *customData) {
//     if (!filter) {
//         filter = filterCompare;
//     }
//     List_t *affirmative = 0;
//     List_t **remaining = list;

//     while (*remaining && filter((*remaining)->payload, (*remaining)->tag, &customData)) {
//         affirmative = List_append(affirmative, (*remaining)->payload, (*remaining)->tag);
//         List_t *lp = *list;
//         *remaining = (*remaining)->next;
//         free(lp);
//     }

//     *list = *remaining;

//     for (List_t **ll = list; (*ll)->next; *ll = (*ll)->next) {
//         if (filter((*ll)->payload, (*ll)->tag, &customData)) {
//             affirmative = List_append(affirmative, (*ll)->next->payload, (*ll)->next->tag);
//             List_t *lp = (*ll)->next;
//             (*ll)->next = (*ll)->next->next;
//             *ll = (*ll)->next;
//             free(lp);
//         }
//     }
//     return affirmative;
// }

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

void *List_apply(List_t *list, applyFunc func, void *customData) {
    while (list) {
        customData = func(&list->payload, list->tag, customData);
        list = list->next;
    }
    return customData;
}

void List_dispose(List_t **list) {
    assert(list && *list);
    if ((*list)->next) {
        List_dispose(&(*list)->next);
    }
    if ((*list)->payload) FREE((*list)->payload);
    FREE(*list);
    *list = NULL;
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

struct Stack_t {
    struct Stack_t *prev;
    void *payload;
    int height;
};

struct Stack_t nullStack = {0, 0};

Stack_t Stack_new(void *top) {
    if (!top) {
        return &nullStack;
    }
    
    Stack_t stack;
    NEW(stack);
    stack->prev = &nullStack;
    stack->payload = top;
    stack->height = 1;
    return stack;
}
void *Stack_pop(Stack_t *stack) {
    Stack_t top = *stack;
    if (top == &nullStack) {
        return 0;
    }
    *stack = top->prev;
    void *data = top->payload;
    FREE(top);
    return data;
}

void Stack_push(Stack_t *stack, void *element) {
    Stack_t top;
    NEW(top);
    top->prev = *stack;    
    top->height = top->prev->height + 1;
    top->payload = element;
    *stack = top;
}

int Stack_size(Stack_t stack) {
    return stack->height;
    // if (stack == &nullStack) {
    //     return stack->height;
    // }
    // return 0;
}

int Stack_empty(Stack_t stack) {
    return stack == &nullStack;
}

void *Stack_top(Stack_t stack) {
    if (stack == &nullStack) {
        return 0;
    }
    return stack->payload;
}

Buffer_t *Buffer_new(size_t bytes_per_item, size_t capacity) {
    Buffer_t *buffer;
    NEW(buffer);
    buffer->buffer = (void*)malloc(bytes_per_item*capacity);
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
    if(RESIZE(buffer->buffer, buffer->bytes_per_item*buffer->capacity + n_items*buffer->bytes_per_item)) {
        return 0;
    };
    buffer->capacity += n_items;
    return 1;
}

int Buffer_write(Buffer_t *buffer, size_t n_items, void *data) {
    size_t to_expand = 0;
    while (buffer->stored + n_items > buffer->capacity) {
        to_expand += BUFFER_EXPAND;
    }
    if (to_expand) {
        if (!Buffer_expand(buffer, to_expand)) {
            return 0;
        }
    }
    memcpy((char*)buffer->buffer + buffer->bytes_per_item*buffer->stored, data, buffer->bytes_per_item*n_items);
    buffer->stored += n_items;
    return 1;
}

void Buffer_dispose(Buffer_t **buffer) {
    assert(buffer && *buffer);
    free((*buffer)->buffer);
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

int Buffer_consume(Buffer_t *buffer, Buffer_t **other) {
    size_t reindex = buffer->stored;
    assert(other && *other && buffer->bytes_per_item == (*other)->bytes_per_item);
    for (int i = 0; i < (*other)->stored; i++) {
        Buffer_write(buffer, (*other)->stored, (*other)->buffer);
    }
    free((*other)->buffer);
    FREE(*other);
    *other = NULL;
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



