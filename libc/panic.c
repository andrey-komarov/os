#include "panic.h"

#include "tty/tty.h"

void run_panic(char* s)
{
  tty_putstr("P A N I C\n");
  tty_putstr(s);
  __asm volatile("cli\n\thlt\n\t");
  __builtin_unreachable();
}
