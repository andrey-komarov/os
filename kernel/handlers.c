#include "kernel/handlers.h"

#include "kernel/kernel.h"
#include "libc/panic.h"
#include "kernel/ioports.h"

void irq_timer()
{
  //  printk("tick!");
}

void irq_kbd()
{
  uint8_t keycode = inb(0x60);
  printk("Key pressed: %x", keycode);
}

void irq_ata1()
{
  uint8_t status = inb(0x3f6);
  printk("ATA1 status: %x", status);
}

void irq_ata2()
{
  printk("ATA2 IRQ");
}

void int_gpf(uint32_t err)
{
  printk("GPF %x", err);
  panic("General Protection Failed");
}

void int_df(uint32_t err)
{
  printk("DF %x", err);
  panic("Double Fault");
}

void int_syscall()
{
  printk("syscall!");
}

void int_page_fault(uint32_t err)
{
  uint32_t cr2;
  __asm volatile("movl %%cr2, %0" : "=r"(cr2));
  printk("Page Fault at %p, err=%x", cr2, err);
  printk("%s %s from %s %s %s",
	 (err & 1) ? "page protection" : "non-present page",
	 (err & 2) ? "WRITE" : "READ",
	 (err & 4) ? "user" : "kernel",
	 (err & 8) ? "reserved field" : "",
	 (err & 16) ? "instruction fetch" : "");
  panic("Page Fault");
}
