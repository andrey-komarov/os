#include "kpmalloc.h"
#include "mem.h"
#include "kernel/mmu.h"
#include "kernel/kernel.h"

#include <stdint.h>

#define KERNELSPACE_PAGES ((1 << (32 - PAGE_BITS)) - KERNEL_VMA / PAGE_SIZE)
#define RESERVED_TABLES (KERNELSPACE_PAGES >> PAGE_DIR_BITS)
#define KERNEL_FIRST_DIR_ENTRY (KERNEL_VMA << (PAGE_BITS + PAGE_TABLE_BITS))

uint32_t kernel_pages_map[KERNELSPACE_PAGES / 32];

pagedir_t kernel_page_dir;
pagetable_t kernel_page_tables[RESERVED_TABLES];

void init_vmem()
{
  int last_kernel_page = (uint32_t)(&end_of_kernel - KERNEL_VMA) / PAGE_SIZE;
  printk("Initializing %d initial kernel pages", last_kernel_page + 1);
  printk("%d kernelspace pages", KERNELSPACE_PAGES);
  int last_kernel_pagedir_entry = last_kernel_page >> PAGE_TABLE_BITS;
  for (int pd_entry = 0; pd_entry <= last_kernel_pagedir_entry; pd_entry++)
    {
      int pd_index = KERNEL_FIRST_DIR_ENTRY + pd_entry;
      pagetable_t *table = &kernel_page_tables[pd_entry];
      kernel_page_dir[pd_index]
        = PD_PRESENT | PD_RW | (uint32_t)&table;
      int skip = pd_entry * PAGE_TABLE_SIZE;
      for (int page = 0; skip + page <= last_kernel_page && page < PAGE_TABLE_SIZE; page++)
        {
          uint32_t phy_addr = (skip + page) << PAGE_SIZE;
          *table[page] = PD_PRESENT | PD_RW | phy_addr;
        }
    }
  //set_page_dir(&kernel_page_dir);
}

void* kpmalloc(size_t pages);
void kpfree(void *addr, size_t pages);
