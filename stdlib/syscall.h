#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_MMAP 90

uint32_t syscall1(uint32_t syscall_num, uint32_t arg1);
uint32_t syscall2(uint32_t syscall_num, uint32_t arg1, uint32_t arg2);
uint32_t syscall3(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);

#endif // _SYSCALL_H
