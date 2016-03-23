#include "bitset.h"

#include "string.h"

int bitset_init(void *bitset, size_t bytes)
{
  memset(bitset, 0, bytes);
  return 0;
}

int bitset_get(void *bitset, size_t pos)
{
  int bucket = pos / BITSET_WIDTH;
  int bit = pos % BITSET_WIDTH;
  uint32_t *data = (uint32_t*)bitset;
  return !!(data[bucket] & (1 << bit));
}

void bitset_set(void *bitset, size_t pos, int newval)
{
  int bucket = pos / BITSET_WIDTH;
  int bit = pos % BITSET_WIDTH;
  uint32_t *data = (uint32_t*)bitset;
  data[bucket] &= ~(1 << bit);
  data[bucket] |= newval << bit;
}
