#ifndef _BITSET_H
#define _BITSET_H

#include <stddef.h>
#include <stdint.h>

#define BITSET_WIDTH 32

int bitset_init(void *bitset, size_t bytes);
int bitset_get(void *bitset, size_t pos);
void bitset_set(void *bitset, size_t pos, int newval);

#endif // _BITSET_H
