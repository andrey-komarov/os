#include "interrupts.h"

void init_interrupts()
{
  
}

void enable_interrupts()
{
  __asm("sti");
}

void disable_interrupts()
{
  __asm("cli");
}
