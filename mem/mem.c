#include "mem.h"
#include "libc/assert.h"
#include "libc/panic.h"
#include "libc/bitset.h"

#include <stddef.h>

// 1 bit per page
static uint32_t bitmap[1 << 15];
static size_t guess;
static size_t total_pages;

static void set(size_t i, int val)
{
  size_t bucket = i / 32;
  size_t pos = i % 32;
  uint32_t old = bitmap[bucket];
  bitmap[bucket] = (old & ~(1 << pos)) | (val << pos);
}

static int get(size_t i)
{
  return !!(bitmap[i / 32] & (1 << (i % 32)));
}

void init_mem(uint32_t mem_upper)
{
  uint32_t eok = (uint32_t)&end_of_kernel - KERNEL_VMA;
  size_t kernel_pages = (eok + PAGE_SIZE - 1) / PAGE_SIZE;
  for (size_t i = 0; i < kernel_pages; i++)
    set(i, 1);
  guess = kernel_pages;
  total_pages = (mem_upper * 1024 + 0x100000) / PAGE_SIZE;
}

void *mem_alloc_page()
{
  for (;!get(guess) ; guess = (guess + 1) % total_pages)
    ;
  set(guess, 1);
  return (void*)(guess * PAGE_SIZE);
}

void mem_free_page(void *page)
{
  assert(((uint32_t)page) % PAGE_SIZE == 0);
  uint32_t pageno = ((uint32_t)page) / PAGE_SIZE;
  assert(get(pageno));
  set(pageno, 0);
}
