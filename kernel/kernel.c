#include "tty/tty.h"
#include "libc/stdio.h"
#include "libc/panic.h"
#include "boot/multiboot.h"
#include "gdt.h"
#include "interrupts.h"
#include "pic.h"
#include "userspace.h"
#include "ata/ata.h"
#include "fs/fs.h"
#include "fs/fat16.h"

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
  init_tty();
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
  ata_identify();
  init_fs();
  enable_interrupts();
  
  /*
  for (int i = 0; i < 20; i++)
    __asm("int $0x50");
  */
  
  printk("o/");
  //fat16_list_all_files();
  
  fat16_fd_t fd;
  if (fat16_open("/HELLO.TXT", &fd) == NULL)
    panic("Failed to open");
  uint8_t buf[4096];
  int r = fat16_read(&fd, buf, 0, fd.file_size);
  if (r < 0)
    panic("Failed to read");
  printk("%s", buf);
  
  //test_userspace();
  //ata_identify();
    
  
  while (1)
    {
      __asm volatile("hlt");
    }
  

  panic("AAAAAAAAAA");
}
