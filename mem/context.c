#include "context.h"

#include "mem/kpmalloc.h"
#include "mem/mem.h"
#include "libc/panic.h"

pagedir_t *context_new()
{
  pagedir_t *ctx = kpmalloc(1);
  for (int ptno = 0; ptno < PAGE_DIR_SIZE; ptno++)
    (*ctx)[ptno] = 0;
  return ctx;
}

void context_clone_present(pagedir_t *dst, pagedir_t *src)
{
  for (size_t ptno = 0; ptno < USER_PAGE_TABLES; ptno++)
    {
      uint32_t pt = (*src)[ptno];
      if (!(pt | PD_PRESENT))
	{
	  (*dst)[ptno] = 0;
	  continue;
	}
      void *phy_table = (void*)(pt & PD_ADDR);
      pagetable_t buf;
      read_phy_page(&buf, phy_table);
      pagetable_t virt_newpt;
      for (int i = 0; i < PAGE_TABLE_SIZE; i++)
	{
	  uint32_t pt_entry = buf[i];
	  if (!(pt_entry | PT_PRESENT))
	    {
	      virt_newpt[i] = 0;
	      continue;
	    }
	  void *ppage = (void*)(pt_entry & PT_ADDR);
	  void *phy_new_page = phymem_alloc_page();
	  char buf2[PAGE_SIZE];
	  read_phy_page(&buf2, ppage);
	  write_phy_page(phy_new_page, &buf2);
	  virt_newpt[i] = (pt_entry & ~PT_ADDR) | (uint32_t)phy_new_page;
	}
      pagetable_t *phy_newpt = phymem_alloc_page();
      write_phy_page(phy_newpt, &virt_newpt);
    }
}

void context_free(pagedir_t *context)
{
  panic("TODO implement me");
  for (int ptno = 0; ptno < USER_PAGE_TABLES; ptno++)
    {
      uint32_t pt = (*context)[ptno];
      if (!(pt | PD_PRESENT))
	continue;
      pagetable_t *table = (pagetable_t*)(pt & PD_ADDR);
      pagetable_t buf;
      read_phy_page(&buf, table);
      for (int pageno = 0; pageno < PAGE_TABLE_SIZE; pageno++)
	{
	  uint32_t page = buf[pageno];
	  if (!(page | PT_PRESENT))
	    continue;
	  kpfree((void*)(page & PT_ADDR), 1);
	}
      kpfree(table, 1);
    }
}
