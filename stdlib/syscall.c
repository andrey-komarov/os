#include "syscall.h"

uint32_t syscall1(uint32_t syscall_num, uint32_t arg1)
{
  uint32_t res;
  __asm volatile(
    "int $0x80\n\t"
    : "=eax"(res)
    : "eax"(syscall_num), "ebx"(arg1)
    :
                 );
  return res;
}

uint32_t syscall3(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
  uint32_t res;
  __asm volatile(
    "int $0x80\n\t"
    : "=eax"(res)
    : "eax"(syscall_num), "ebx"(arg1), "ecx"(arg2), "edx"(arg3)
    :
                 );
  return res;
}
