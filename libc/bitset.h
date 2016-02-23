#ifndef _BITSET_H
#define _BITSET_H

#include <stddef.h>
#include <stdint.h>

#define BITSET_WIDTH 32
#define BITSET(name, size) union                        \
  {                                                     \
    char name##_raw_[size + sizeof(bitset_t)];          \
      bitset_t name;                                    \
  };                                                    \
  int name##_init_flag_ = bitset_init(name, size)       \



typedef struct bitset_t
{
  size_t size;
  size_t hint;
  uint32_t data[];
} bitset_t;

int bitset_init(bitset_t *bitset, size_t bytes);
int bitset_get(bitset_t *bitset, size_t pos);
void bitset_set(bitset_t *bitset, size_t pos, int newval);

#endif // _BITSET_H
