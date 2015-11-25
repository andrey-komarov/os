#include "interrupts.h"
#include "gdt.h"
#include "kernel.h"

static idt_entry_t idt[IDT_SIZE];

void set_idt(uint16_t size, idt_entry_t* addr)
{
  struct {
    uint16_t size;
    uint32_t addr;
  } __attribute__((packed)) gdtr = {.size = size - 1, .addr = (uint32_t)addr};

  printk("Ready to launch!");
  __asm volatile("lidt (%0)": : "a"(&gdtr));
  printk("Liftoff!");
}

void init_interrupts()
{
  for (int i = 0; i < IDT_SIZE; i++)
    {
      idt[i].offset_1 = ((uint32_t)interrupt_handler) & 0xffff;
      idt[i].selector = KERNEL_CODE_SEGMENT * 8;
      idt[i].zero = 0;
      // 0x80 - present
      // type = 32-bit interrupt gate
      idt[i].type_attr = 0x8e;
      idt[i].offset_2 = (((uint32_t)interrupt_handler) >> 16) & 0xffff;
    }
  
  set_idt(sizeof(idt), idt);
}

void enable_interrupts()
{
  __asm("sti");
}

void disable_interrupts()
{
  __asm("cli");
}
