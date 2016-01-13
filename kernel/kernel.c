#include "tty/tty.h"
#include "libc/stdio.h"
#include "libc/panic.h"
#include "boot/multiboot.h"
#include "gdt.h"
#include "interrupts.h"
#include "pic.h"
#include "userspace.h"
#include "ata/ata.h"

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
  
  if (!(mbi->flags & (1 << 5)))
    panic("expected EFL metadata");
  
  printk("ELF info: num = %u, size = %u, addr = %p, shndx = %p", 
         mbi->u.elf_sec.num, mbi->u.elf_sec.size, mbi->u.elf_sec.addr, 
         mbi->u.elf_sec.shndx);
  
  disable_interrupts();
  init_gdt();
  init_interrupts();
  pic_remap(PIC1, PIC2);
  init_ata();
  enable_interrupts();
  
  /*
  for (int i = 0; i < 20; i++)
    __asm("int $0x50");
  */
  
  printk("o/");
  
  //test_userspace();
  ata_identify();
  uint16_t buf[256];
  ata_read(0, buf);
  printk("%s", buf);
    
  
  while (1)
    {
      __asm volatile("hlt");
    }
  

  panic("AAAAAAAAAA");
}
