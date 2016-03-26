#ifndef _KPMALLOC_H
#define _KPMALLOC_H

#include <stddef.h>

#include "kernel/mmu.h"

#define KERNELSPACE_PAGES ((1 << (32 - PAGE_BITS)) - KERNEL_VMA / PAGE_SIZE)
#define RESERVED_TABLES (KERNELSPACE_PAGES >> PAGE_DIR_BITS)
#define KERNEL_FIRST_DIR_ENTRY (KERNEL_VMA >> (PAGE_BITS + PAGE_TABLE_BITS))


extern pagetable_t kernel_page_tables[RESERVED_TABLES];

void init_vmem();

void* kpmalloc(size_t pages);
void kpfree(void *addr, size_t pages);

#endif // _KPMALLOC_H
