#ifndef utilities_H
#define utilities_H

#define BUFFER_OFFSET(i) ((char *)0 + (i));

int strpos(char *haystack, char *needle);
void debug(char *file, int line, char *format, ...);
char *copyString(char *in);

#define ECHOERR(X, ...) (debug(__FILE__, __LINE__, X, __VA_ARGS__))

#endif