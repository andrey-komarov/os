#include "context.h"

#include "mem/kpmalloc.h"
#include "mem/mem.h"

pagedir_t *context_new()
{
  pagedir_t *ctx = kpmalloc(1);
  for (int ptno = 0; ptno < PAGE_DIR_SIZE; ptno++)
    (*ctx)[ptno] = 0;
  return ctx;
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
