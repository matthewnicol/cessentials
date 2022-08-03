#ifndef GLGAME_EXCEPT_H
#define GLGAME_EXCEPT_H

#include <setjmp.h>
#define T Except_T
typedef struct T {
    const char *reason;
} T;

// Exported Types
typedef struct Except_Frame Except_Frame;
struct Except_Frame {
    Except_Frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const T *exception;
};

enum { Except_entered=0, Except_raised,
    Except_handled, Except_finalized };


// Exported Variables
extern Except_Frame *Except_stack;
extern const Except_T Assert_Failed;

// Exported Functions
void Except_raise(const T *e, const char *file, int line);
void Except_raise_msg(const char *file, int line, const char *msg);

// Exported Macros
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RAISE_MSG(e) Except_raise_msg(__FILE__, __LINE__, e)
#define RERAISE Except_raise(Except_frame.exception, \
    Except_frame.file, Except_frame.line)

#define RETURN switch (Except_stack = Except_stack->prev,0) default: return

#define TRY do { \
volatile int Except_flag; \
Except_Frame Except_frame; \
Except_frame.prev = Except_stack; \
Except_stack = &Except_frame; \
Except_flag = setjmp(Except_frame.env); \
if (Except_flag == Except_entered) {

#define EXCEPT(e) \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} else if (Except_frame.exception == &(e)) { \
Except_flag = Except_handled;

#define ELSE \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} else { \
Except_flag = Except_handled

#define FINALLY \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} { \
if (Except_flag == Except_entered) \
Except_flag = Except_finalized;

#define END_TRY \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} if (Except_flag == Except_raised) RERAISE; \
} while (0)

#undef T
#endif
