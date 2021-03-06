#include "kpmalloc.h"
#include "mem.h"
#include "kernel/kernel.h"
#include "libc/bitset.h"
#include "libc/panic.h"
#include "libc/assert.h"

#include <stdint.h>

static uint32_t kernel_pages_map[KERNELSPACE_PAGES / 32];

pagedir_t kernel_page_dir;
pagetable_t kernel_page_tables[RESERVED_TABLES];
pagedir_t *kernel_page_dir_phy = (pagedir_t*)((uint32_t)&kernel_page_dir - KERNEL_VMA);

void init_vmem()
{
  int last_kernel_page = (uint32_t)(&end_of_kernel - KERNEL_VMA) / PAGE_SIZE;
  int last_kernel_pagedir_entry = last_kernel_page >> PAGE_TABLE_BITS;
  for (size_t pd_entry = 0; pd_entry < RESERVED_TABLES; pd_entry++)
    {
      size_t pd_index = KERNEL_FIRST_DIR_ENTRY + pd_entry;
      pagetable_t *phytable = virt_to_phy(&kernel_page_tables[pd_entry]);
      kernel_page_dir[pd_index]
        = PD_PRESENT | PD_RW | (uint32_t)phytable;
      int skip = pd_entry * PAGE_TABLE_SIZE;
      for (int page = 0; skip + page <= last_kernel_page && page < PAGE_TABLE_SIZE; page++)
        {
          uint32_t phy_addr = ((skip + page) << PAGE_BITS);
          kernel_page_tables[pd_entry][page] = PT_PRESENT | PT_RW | phy_addr;
        }
    }
  for (int i = 0; i <= last_kernel_page; i++)
    bitset_set(kernel_pages_map, i, 1);
  // Reserve last page for `{read,write}_phy_page`
  bitset_set(kernel_pages_map, sizeof(kernel_pages_map) * 8 - 1, 1);
  set_page_dir_rewrite_kernel(kernel_page_dir);
}

static uint32_t find_free_consecutive_single(size_t pages, size_t guess)
{
  size_t found = 0;
  while (guess < KERNELSPACE_PAGES)
    {
      while (found < pages && guess < KERNELSPACE_PAGES && !bitset_get(kernel_pages_map, guess))
	{
	  found++;
	  guess++;
	}
      if (found == pages)
	return guess - pages;
      guess++;
      found = 0;
    }
  return 0;
}

static uint32_t find_free_consecutive(size_t pages)
{
  static uint32_t guess = 0;
  uint32_t first = find_free_consecutive_single(pages, guess);
  if (first > 0)
    {
      guess = first + pages;
      return first;
    }
  uint32_t second = find_free_consecutive_single(pages, 0);
  if (second > 0)
    {
      guess = second + pages;
      return second;
    }
  else
    {
      panic("Out of (virtual) kernel memory");
    }
}

void* kpmalloc(size_t pages)
{
  uint32_t first = find_free_consecutive(pages);
  for (size_t i = 0; i < pages; i++)
    {
      size_t pageno = first + i;
      bitset_set(kernel_pages_map, pageno, 1);
      size_t tableno = pageno / PAGE_TABLE_SIZE;
      uint32_t phypage = (uint32_t)phymem_alloc_page();
      uint32_t *p = &kernel_page_tables[tableno][pageno % PAGE_TABLE_SIZE];
      assert(!(*p & PT_PRESENT));
      *p = phypage | PT_PRESENT | PT_RW;
    }
  return (void*)(KERNEL_VMA + first * PAGE_SIZE);
}

void kpfree1(void *addr)
{
  uint32_t page = (uint32_t)addr;
  uint32_t pageno = page / PAGE_SIZE - USER_PAGE_TABLES * PAGE_TABLE_SIZE;
  bitset_set(kernel_pages_map, pageno, 0);
  uint32_t tableno = pageno / PAGE_TABLE_SIZE;
  kernel_page_tables[tableno][pageno % PAGE_TABLE_SIZE]
    &= ~PT_PRESENT;
  phymem_free_page(virt_to_phy(addr));
}

void kpfree(void *addr, size_t pages)
{
  uint32_t page = (uint32_t)addr;
  for (size_t i = 0; i < pages; i++)
    {
      kpfree1((void*)page);
      page += PAGE_SIZE;
    }
}
