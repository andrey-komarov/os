#include "context.h"

#include "mem/kpmalloc.h"
#include "mem/mem.h"

pagedir_t *context_new()
{
  return kpmalloc(1);
}

void context_init_kernel_pages(pagedir_t *context)
{
  printk("USER_PAGE_TABLES = %d", USER_PAGE_TABLES);
  for (int ptno = 0; ptno < USER_PAGE_TABLES; ptno++)
    (*context)[ptno] = 0;
  printk("cur page dir = %p", *virt_current_pagedir);
  for (int ptno = USER_PAGE_TABLES; ptno < PAGE_DIR_SIZE; ptno++)
    (*context)[ptno] = (*virt_current_pagedir)[ptno];
  printk("done");
}

void context_free(pagedir_t *context)
{
  for (int ptno = 0; ptno < USER_PAGE_TABLES; ptno++)
    {
      uint32_t pt = (*context)[ptno];
      if (!(pt | PD_PRESENT))
	continue;
      pagetable_t *table = (pagetable_t*)(pt & PD_ADDR);
      for (int pageno = 0; pageno < PAGE_TABLE_SIZE; pageno++)
	{
	  uint32_t page = (*table)[pageno];
	  if (!(page | PT_PRESENT))
	    continue;
	  kpfree((void*)(page & PT_ADDR), 1);
	}
      kpfree(table, 1);
    }
}
