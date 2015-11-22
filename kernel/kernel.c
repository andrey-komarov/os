#include "tty/tty.h"
#include "libc/stdio.h"
#include "libc/panic.h"

void kernel_main()
{
  tty_cls();
  tty_putstr("Hello kernel!\n");

  int* addr = 0x200000;
  char buf[100];
  while (1)
    {
      if ((((int)addr) & (0x1000 - 1)) == 0)
        {
          sprintf(buf, "Checking %p %llx\n", addr, 1234123414134LL);
          tty_putstr(buf);
        }
      *addr = 42;
      if (*addr != 42)
        break;
      addr++;
    }
  sprintf(buf, "Failed at %p\n", addr);
  tty_putstr(buf);

  panic("AAAAAAAAAA");
}
