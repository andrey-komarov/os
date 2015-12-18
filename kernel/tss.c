#include "tss.h"
#include "gdt.h"

tss_entry_t tss_entry;

void tss_flush()
{
  __asm volatile(
    "movw %0, %%ax\n\t"
    "ltr %%ax\n\t"
    :
    : "i"(TSS_SEGMENT * sizeof(gdt_entry_t))
    : "eax");
}
