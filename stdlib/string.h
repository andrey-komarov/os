#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

size_t strlen(char* s);

void *memcpy(void *dest, const void *src, size_t n);

char *strcpy(char *dest, const char *src);

#endif // _STRING_H
