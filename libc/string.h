#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

size_t strlen(char* s);

void *memcpy(void *dest, const void *src, size_t n);

char *strcpy(char *dest, const char *src);

int strcmp(char *s1, char *s2);

#endif // _STRING_H
