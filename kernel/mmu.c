#include "mmu.h"

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
  void *phy = virt_to_phy(pagedir);
  virt_current_pagedir = pagedir;
  __asm volatile("movl %0, %%cr3"
                 :
                 : "eax"(phy));
}

void* virt_to_phy(void* addr)
{
  int page_table_no = (uint32_t)addr >> (PAGE_BITS + PAGE_TABLE_BITS);
  uint32_t page_dir_entry = (*virt_current_pagedir)[page_table_no];
  if (page_dir_entry & PD_HUGE)
    {
      page_dir_entry >>= 22;
      page_dir_entry <<= 22;
      return (void*)(page_dir_entry + (uint32_t)addr % (1 << 22));
    }
  else
    {
      int page_no = ((uint32_t)addr >> PAGE_BITS) & ((1 << PAGE_TABLE_BITS) - 1);
      pagetable_t *table = (pagetable_t*)((uint32_t)virt_current_pagedir[page_table_no] & ~((1 << (32 - PAGE_BITS)) - 1));
      uint32_t page = *table[page_no] & ~((1 << (32 - PAGE_BITS)) - 1);
      return (void*)(page + (uint32_t)addr % PAGE_SIZE);
    }
}
