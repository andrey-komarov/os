#include "string.h"

size_t strlen(char* s)
{
  size_t i;
  for (i = 0; ;i++)
    if (s[i] == '\0')
      return i;
}

void *memcpy(void *dest, const void *src, size_t n)
{
  char* dest_c = dest;
  const char* src_c = src;
  for (size_t i = 0; i < n; i++)
    dest_c[i] = src_c[i];
  return dest;
}

char *strcpy(char *dest, const char *src)
{
  for (size_t i = 0; src[i] != '\0'; i++)
    dest[i] = src[i];
  return dest;
}

void *memset(void *s, int c, size_t n)
{
  for (size_t i = 0; i < n; i++)
    ((char*)s)[i] = c;
  return s;
}
