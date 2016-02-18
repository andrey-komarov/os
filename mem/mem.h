#ifndef _MEM_H
#define _MEM_H

#include <stdint.h>

#define KERNEL_VMA 0xc0100000
#define PAGE_SIZE 4096
#define PAGE_BITS 12

extern char end_of_kernel;

void init_mem(uint32_t mem_upper);

void *mem_alloc_page();
void mem_free_page(void *page);

#endif // _MEM_H
