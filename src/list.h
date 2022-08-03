#ifndef GLGAME_LIST_H
#define GLGAME_LIST_H

#include <stdarg.h>

#define BUFFER_EXPAND 200

typedef struct List_t {
    void *payload;
    int tag; // Possibly unused
    struct List_t *next;
} List_t;

List_t *List_append(List_t *list, void *payload, int tag);
List_t *List_list(void *x, ...);
List_t *List_taggedList(int tag, void *x, ...);
typedef void*(*applyFunc)(void**, int, void*);
void *List_apply(List_t *list, applyFunc func, void *customData);
List_t *List_new(void *payload, int tag);
// List_t *List_partition(List_t **list, int (*filter)(void *payload, int tag, void **customData), void *customData);


void List_dispose(List_t **list);
void List_orphan(List_t **list);

List_t *List_drop(List_t *l, void* payload);
//List_t *List_pop(List_t **list);
List_t *List_popLeft(List_t **list);
List_t *List_popRight(List_t **list);

typedef struct Stack_t *Stack_t;

Stack_t Stack_new(void *top);
void *Stack_pop(Stack_t *stack);
void *Stack_top(Stack_t stack);
void Stack_push(Stack_t *stack, void *top);
int Stack_size(Stack_t stack);
int Stack_empty(Stack_t stack);

typedef struct Buffer_t {
    size_t bytes_per_item;
    size_t stored;
    size_t capacity;
    void *buffer;
} Buffer_t;

Buffer_t *Buffer_new(size_t bytes_per_item, size_t capacity);
void Buffer_dispose(Buffer_t **buffer);
int Buffer_expand(Buffer_t *buffer, size_t nbytes);
int Buffer_write(Buffer_t *buffer, size_t n_items, void *data);
void *Buffer_orphan(Buffer_t **buffer);

List_t *Buffer_pluck(Buffer_t *buffer, int items, int *indexes);
int Buffer_consume(Buffer_t *buffer, Buffer_t **other);
int Buffer_index(Buffer_t *buffer, void *data);

#define List_pop(L) List_popLeft(L)

#endif
