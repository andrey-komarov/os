#include "bitset.h"

#include "string.h"

int bitset_init(bitset_t *bitset, size_t bytes)
{
  bitset->size = (bytes + 3) / 4;
  bitset->hint = 0;
  memset(bitset->data, 0, bytes);
  return 1;
}

int bitset_get(bitset_t *bitset, size_t pos)
{
  return !!(bitset->data[pos / BITSET_WIDTH] & (1 << (pos % BITSET_WIDTH)));
}

void bitset_set(bitset_t *bitset, size_t pos, int newval)
{
  int bucket = pos / BITSET_WIDTH;
  int bit = pos % BITSET_WIDTH;
  bitset->data[bucket] &= ~(1 << bit);
  bitset->data[bucket] |= newval << bit;
}
