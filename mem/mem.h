#ifndef _MEM_H
#define _MEM_H

#include <stdint.h>
#include "kernel/mmu.h"

#define KERNEL_VMA 0xc0000000
#define PAGE_SIZE 4096
#define PAGE_BITS 12
#define USER_PAGE_TABLES (KERNEL_VMA / PAGE_SIZE / PAGE_TABLE_SIZE)

extern char end_of_kernel;

void init_mem(uint32_t mem_upper);

void *phymem_alloc_page();
void phymem_free_page(void *page);

#endif // _MEM_H
