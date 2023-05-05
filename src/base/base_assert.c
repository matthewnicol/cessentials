#include <stdarg.h>
#include <stdio.h>
#include "base.h"

const Except_T Assert_Failed = { "Assertion failed" };

void (assert)(int e) {
    assert(e);
}

void _assert_with_message(int e, char *file, int line, char *format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
    const Except_T failure = {buffer};
    Except_raise(&failure, file, line);
    va_end(args);
}