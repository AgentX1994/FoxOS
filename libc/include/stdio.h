#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>

#define EOF (-1)
#include <sys/printf.h>

#ifdef __cplusplus
extern "C" {
#endif

int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
