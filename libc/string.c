#include "string.h"

size_t strlen(char* s)
{
  size_t i;
  for (i = 0; ;i++)
    if (s[i] == '\0')
      return i;
}
