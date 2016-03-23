#include "mem.h"
#include "libc/assert.h"
#include "libc/panic.h"
#include "libc/bitset.h"

#include <stddef.h>

// 1 bit per page
static uint32_t bitmap[1 << 15];
static size_t guess;
static size_t total_pages;

void init_mem(uint32_t mem_upper)
{
  uint32_t eok = (uint32_t)&end_of_kernel - KERNEL_VMA;
  size_t kernel_pages = (eok + PAGE_SIZE - 1) / PAGE_SIZE;
  for (size_t i = 0; i < kernel_pages; i++)
    bitset_set(bitmap, i, 1);
  guess = kernel_pages;
  total_pages = (mem_upper * 1024 + 0x100000) / PAGE_SIZE;
}

void *phymem_alloc_page()
{
  for (;bitset_get(bitmap, guess) ; guess = (guess + 1) % total_pages)
    ;
  bitset_set(bitmap, guess, 1);
  return (void*)(guess * PAGE_SIZE);
}

void phymem_free_page(void *page)
{
  assert(((uint32_t)page) % PAGE_SIZE == 0);
  uint32_t pageno = ((uint32_t)page) / PAGE_SIZE;
  assert(bitset_get(bitmap, pageno));
  bitset_set(bitmap, pageno, 0);
}
