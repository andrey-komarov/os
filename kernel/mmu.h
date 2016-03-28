#ifndef _MMU_H
#define _MMU_H

#include <stdint.h>
#include <stddef.h>

#include "mem/mem.h"

#define PAGE_SIZE 4096
#define PAGE_BITS 12

#define PAGE_TABLE_BITS 10
#define PAGE_TABLE_SIZE 1024
#define PAGE_DIR_BITS 10
#define PAGE_DIR_SIZE 1024

#define PD_PRESENT (1 << 0)
#define PD_RW (1 << 1)
#define PD_USER (1 << 2)
#define PD_HUGE (1 << 7)
#define PD_ADDR (~((1 << PAGE_DIR_BITS) - 1))

#define PT_PRESENT (1 << 0)
#define PT_RW (1 << 1)
#define PT_USER (1 << 2)
#define PT_WRITETHROUGH (1 << 3)
#define PT_NOCACHE (1 << 4)
#define PT_ADDR (~((1 << PAGE_TABLE_BITS) - 1))

typedef uint32_t pagedir_t[PAGE_DIR_SIZE] __attribute__((aligned(PAGE_SIZE)));
typedef uint32_t pagetable_t[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

extern pagedir_t bootp_page_dir;
extern pagedir_t *virt_current_pagedir;

void set_page_dir(pagedir_t *pagedir);
void set_page_dir_rewrite_kernel(pagedir_t *pagedir);
void read_phy_page(void *vdst, void *ppage);
void write_phy_page(void *pdst, void *vpage);
void read_phy(void *vdst, void *paddr, size_t size);
void write_phy(void *pdst, void *vaddr, size_t size);
void invlpg(void *page);

void* virt_to_phy(void* addr);

#endif // _MMU_H
