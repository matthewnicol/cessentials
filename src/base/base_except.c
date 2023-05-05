#include <stdlib.h>
#include <stdio.h>
#include "base.h"

Except_Frame *Except_stack = NULL;

void Except_raise_msg(const char *file, int line, const char *msg) {
    struct Except_T e = {msg};
    Except_raise(&e, file, line);
}

void Except_raise(const Except_T *e, const char *file, int line) {
    Except_Frame *p = Except_stack;
    assert(e);

    if (p == NULL) {
        fflush(stdout);

        // announce an uncaught exception
        fprintf(stderr, "Uncaught exception");
        if (e->reason)
            fprintf(stderr, " %s", e->reason);
        else
            fprintf(stderr, " at 0x%p", e);
        if (file && line > 0)
            fprintf(stderr, " raised at %s:%d\n", file, line);
        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        abort();

    }
    p->exception = e;
    p->file = file;
    p->line = line;
    Except_stack = Except_stack->prev;
    longjmp(p->env, Except_raised);
}