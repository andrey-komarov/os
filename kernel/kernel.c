#include "tty/tty.h"

void kernel_main()
{
  tty_cls();
  tty_putstr("Hello kernel");
}
