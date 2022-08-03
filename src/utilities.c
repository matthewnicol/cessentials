#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "core/structures/control/mem.h"
#include "utilities.h"

int strpos(char *haystack, char *needle) {
    char *p = haystack;
    int i = 0;
    while (*(p + i) != '\0') {
        if (strncmp(p+i, needle, strlen(needle)) == 0) {
            return i;
        }
        i++;
    }
    return -1;
}

void debug(char *file, int line, char *format, ...) {

    printf("(DEBUG) %s[%d]: ", &file[max(strpos(file, "src"), 0)], line);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
}

char *copyString(char *in) {
    char *c;
    NEWN(c, strlen(in)+1);
    sprintf(c, "%s", in); 
    return c;
}