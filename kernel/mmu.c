#include <stddef.h>
#include "mmu.h"
#include "kernel/interrupts.h"
#include "libc/assert.h"
#include "mem/kpmalloc.h"
#include "libc/string.h"
#include "mem/kpmalloc.h"

/*
 * 31    24 23    16 15     8 7      0
 * AAAAAAAA AAAAAAAA AAAAXXXG S0ADWURP
 */
pagedir_t bootp_page_dir = {
  [0] = PD_PRESENT | PD_RW | PD_HUGE,
  [0xc0000000 >> 22] = PD_PRESENT | PD_RW | PD_HUGE,
};

pagedir_t *virt_current_pagedir = &bootp_page_dir;

void set_page_dir(pagedir_t *pagedir)
{
  for (int i = USER_PAGE_TABLES; i < PAGE_DIR_SIZE; i++)
    (*pagedir)[i] = (*virt_current_pagedir)[i];
  set_page_dir_rewrite_kernel(pagedir);
}

void set_page_dir_rewrite_kernel(pagedir_t *pagedir)
{
  void *phy = virt_to_phy(*pagedir);
  virt_current_pagedir = pagedir;
  __asm volatile("movl %0, %%cr3"
                 :
                 : "eax"(phy));
}

void* virt_to_phy(void* addr)
{
  int page_table_no = (uint32_t)addr >> (PAGE_BITS + PAGE_TABLE_BITS);
  uint32_t page_dir_entry = (*virt_current_pagedir)[page_table_no];
  assert(page_dir_entry & PD_PRESENT);
  if (page_dir_entry & PD_HUGE)
    {
      page_dir_entry >>= 22;
      page_dir_entry <<= 22;
      return (void*)(page_dir_entry + (uint32_t)addr % (1 << 22));
    }
  else
    {
      uint32_t page_no = ((uint32_t)addr >> PAGE_BITS) & ((1 << PAGE_TABLE_BITS) - 1);
      pagetable_t *table = (pagetable_t*)(page_dir_entry & PD_ADDR);
      // Kernel page tables are not reallocated so this "nice trick"
      // allows us to get virtual address for `table` from physical
      // one
      table = (pagetable_t*)(((uint32_t)table) + KERNEL_VMA);
      uint32_t page_entry = (*table)[page_no];
      uint32_t page = page_entry & PT_ADDR;
      return (void*)(page + (uint32_t)addr % PAGE_SIZE);
    }
}

void invlpg(void *page)
{
  __asm("invlpg (%0)": : "eax"(page));
}

#define TMP_PHY_PAGE_MAPPING ((void*)0xfffff000)
static void mount_tmp_page(void *ppage)
{
  invlpg(ppage);
  kernel_page_tables[KERNELSPACE_PAGES / PAGE_TABLE_SIZE - 1][PAGE_TABLE_SIZE - 1] = PT_RW | PT_PRESENT | (uint32_t)ppage;
}

void read_phy_page(void *vdst, void *ppage)
{
  return read_phy(vdst, ppage, PAGE_SIZE);
}

void write_phy_page(void *pdst, void *vsrc)
{
  return write_phy(pdst, vsrc, PAGE_SIZE);
}

void read_phy(void *vdst, void *paddr, size_t size)
{
  uint32_t ppage = (uint32_t)paddr / PAGE_SIZE * PAGE_SIZE;
  assert((uint32_t)paddr + size <= ppage + PAGE_SIZE);
  uint32_t off = (uint32_t)paddr % PAGE_SIZE;

  disable_interrupts();
  mount_tmp_page(ppage);
  memcpy(vdst, TMP_PHY_PAGE_MAPPING + off, size);
  enable_interrupts();
}

void write_phy(void *pdst, void *vaddr, size_t size)
{
  uint32_t ppage = (uint32_t)pdst / PAGE_SIZE * PAGE_SIZE;
  assert((uint32_t)pdst + size <= ppage + PAGE_SIZE);
  uint32_t off = (uint32_t)pdst % PAGE_SIZE;

  disable_interrupts();
  mount_tmp_page(ppage);
  memcpy(((uint8_t*)TMP_PHY_PAGE_MAPPING) + off, vaddr, size);
  enable_interrupts();
}
