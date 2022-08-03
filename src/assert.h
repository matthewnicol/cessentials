#ifndef GLGAME_ASSERT_H
#define GLGAME_ASSERT_H

#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#include "core/structures/control/except.h"
extern void assert(int e);

#define assert(e) ((void)((e) || (RAISE(Assert_Failed), 0)))

#endif

#endif
