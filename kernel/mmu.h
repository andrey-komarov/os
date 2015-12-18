#ifndef _MMU_H
#define _MMU_H

#include <stdint.h>

#define PAGE_TABLE_SIZE 1024
#define PAGE_DIR_SIZE 1024

#define PD_PRESENT (1 << 0)
#define PD_RW (1 << 1)
#define PD_USER (1 << 2)
#define PD_HUGE (1 << 7)


extern uint32_t bootp_page_dir[PAGE_DIR_SIZE];

#endif // _MMU_H
