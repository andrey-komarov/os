#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

size_t strlen(char* s);

void *memcpy(void *dest, const void *src, size_t n);

char *strcpy(char *dest, const char *src);

int strcmp(char *s1, char *s2);

void *memset(void *s, int c, size_t n);

#endif // _STRING_H
