#ifndef _MMU_H
#define _MMU_H

#include <stdint.h>

#define PAGE_SIZE 4096

#define PAGE_TABLE_BITS 10
#define PAGE_TABLE_SIZE 1024
#define PAGE_DIR_BITS 10
#define PAGE_DIR_SIZE 1024

#define PD_PRESENT (1 << 0)
#define PD_RW (1 << 1)
#define PD_USER (1 << 2)
#define PD_HUGE (1 << 7)

#define PT_PRESENT (1 << 0)
#define PT_RW (1 << 1)
#define PT_USER (1 << 2)

typedef uint32_t pagedir_t[PAGE_DIR_SIZE] __attribute__((aligned(PAGE_SIZE)));
typedef uint32_t pagetable_t[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_SIZE)));

extern pagedir_t bootp_page_dir;

void set_page_dir(pagedir_t *pagedir);

#endif // _MMU_H
