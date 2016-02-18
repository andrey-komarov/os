#include "mmu.h"

/*
 * 31    24 23    16 15     8 7      0
 * AAAAAAAA AAAAAAAA AAAAXXXG S0ADWURP
 */
pagedir_t bootp_page_dir = {
  [0] = PD_PRESENT | PD_RW | PD_HUGE,
  [0xc0000000 >> 22] = PD_PRESENT | PD_RW | PD_HUGE,
};

void set_page_dir(pagedir_t *pagedir)
{
  __asm volatile("movl %0, %%cr3"
                 :
                 : "eax"(pagedir));
}
