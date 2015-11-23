#include "tty/tty.h"
#include "libc/stdio.h"
#include "libc/panic.h"
#include "boot/multiboot.h"

static char printk_buf[1 << 13];
int printk(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  size_t w = vsprintf(printk_buf, format, args);
  if (w >= sizeof(printk_buf))
    panic("printk buffer overflow");
  tty_putstr(printk_buf);
  tty_putchar('\n');
  return w;
}

void kernel_main(unsigned long magic, multiboot_info_t *mbi)
{
  tty_cls();
  printk("Hello kernel!");
  
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("bad bootloader magic");

  printk("flags = %p", mbi->flags);

  printk("mem_lower = %u KB, mem_upper = %u KB", mbi->mem_lower, mbi->mem_upper);

  panic("AAAAAAAAAA");
}
