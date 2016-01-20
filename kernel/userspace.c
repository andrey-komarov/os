#include "userspace.h"

#include "gdt.h"
#include "mmu.h"

void test_function()
{

}

void test_userspace()
{
  bootp_page_dir[((uint32_t)0x80000000) >> 22] = 
    0x400000 | PD_PRESENT | PD_RW | PD_HUGE | PD_USER;
  int i;
  for (i = 0; i < 100; i++)
    *(unsigned char*)(0x80000000 + i) = 0x90;
  *(unsigned char*)0x80000010 = 0xFA;
  __asm volatile(
    "movw %0, %%ax\n\t"
    "movw %%ax, %%ds\n\t"
    "movw %%ax, %%es\n\t"
    "movw %%ax, %%fs\n\t"
    "movw %%ax, %%gs\n\t"
    "movl %%esp, %%eax\n\t"
    "pushl %0\n\t"
    //"pushl %%eax\n\t"
    "pushl $0x80001000\n\t"
    "pushf\n\t"
    "pushl %1\n\t"
    "pushl $0x80000000\n\t"
    "iret\n\t"
    :
    : "i"(USER_DATA_SEGMENT * sizeof(gdt_entry_t) + 3)
    , "i"(USER_CODE_SEGMENT * sizeof(gdt_entry_t) + 3)
    : "eax");
  __builtin_unreachable();
}
