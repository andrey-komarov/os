#include "mmu.h"
#include "kernel/interrupts.h"
#include "libc/assert.h"

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
  printk("SET1 %p", pagedir);
  printk("SET2 %p", *pagedir);
  printk("SET2 %p", **pagedir);
  void *phy = virt_to_phy(*pagedir);
  printk(":)))");
  virt_current_pagedir = pagedir;
  __asm volatile("movl %0, %%cr3"
                 :
                 : "eax"(phy));
}

void* virt_to_phy(void* addr)
{
  int page_table_no = (uint32_t)addr >> (PAGE_BITS + PAGE_TABLE_BITS);
  printk("HERE1");
  uint32_t page_dir_entry = (*virt_current_pagedir)[page_table_no];
  assert(page_dir_entry & PD_PRESENT);
  printk("HERE2");
  if (page_dir_entry & PD_HUGE)
    {
      printk("HUGE");
      page_dir_entry >>= 22;
      page_dir_entry <<= 22;
      return (void*)(page_dir_entry + (uint32_t)addr % (1 << 22));
    }
  else
    {
      printk("SMALL");
      uint32_t page_no = ((uint32_t)addr >> PAGE_BITS) & ((1 << PAGE_TABLE_BITS) - 1);
      printk("HERE3 pageno=%d pagetableno=%d", page_no, page_table_no);
      pagetable_t *table = page_dir_entry & PD_ADDR;
      // Kernel page tables are not reallocated so this "nice trick"
      // allows us to get virtual address for `table` from physical
      // one
      table = (pagetable_t*)(((uint32_t)table) + KERNEL_VMA);
      printk("table = %p %p %x", addr, table, (*virt_current_pagedir)[page_table_no]);
      uint32_t page_entry = (*table)[page_no];
      printk("pe = %x", page_entry);
      uint32_t page = page_entry & PT_ADDR;
      printk("LOL %p", page);
      return (void*)(page + (uint32_t)addr % PAGE_SIZE);
    }
}
