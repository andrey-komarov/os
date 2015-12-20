#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);

int printf(const char *format, ...);
int vprintf(const char *format, va_list ap);

#endif // _STDIO_H
