#include "tty/tty.h"
#include "libc/stdio.h"
#include "libc/panic.h"

void kernel_main()
{
  tty_cls();
  char buf[100];
  tty_putstr("Initial hello\n");
  for (int i = 0; i < 10; i++)
    {
      sprintf(buf, "Hello kernel %d %p %s\n", i, &kernel_main, "wtf");
      tty_putstr(buf);
    }
  panic("AAAAAAAAAA");
}
