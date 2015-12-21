#include "interrupts.h"
#include "gdt.h"
#include "kernel.h"
#include "pic.h"

static idt_entry_t idt[IDT_SIZE];

void set_idt(uint16_t size, idt_entry_t* addr)
{
  struct {
    uint16_t size;
    uint32_t addr;
  } __attribute__((packed)) gdtr = {.size = size - 1, .addr = (uint32_t)addr};

  __asm volatile("lidt (%0)": : "a"(&gdtr));
  printk("IDT set");
}

static void isr_init(idt_entry_t* idt, uint32_t addr, int dpl)
{
      idt->offset_1 = ((uint32_t)addr) & 0xffff;
      idt->selector = KERNEL_CODE_SEGMENT * 8;
      idt->zero = 0;
      // 0x80 - present
      // type = 32-bit interrupt gate
      idt->type_attr = 0x8e | (dpl << 5);
      idt->offset_2 = (((uint32_t)addr) >> 16) & 0xffff;
}

void init_interrupts()
{
  for (int i = 0; i < IDT_SIZE; i++)
    {
      isr_init(&idt[i], (uint32_t)interrupt_handler, 0);
    }
        
  isr_init(&idt[PIC1 + 0], (uint32_t)irq0_handler, 0);
  isr_init(&idt[PIC1 + 1], (uint32_t)irq1_handler, 0);
  isr_init(&idt[INT_DOUBLE_FAULT], (uint32_t)irq8_handler, 0);
  isr_init(&idt[INT_GPF], (uint32_t)irq13_handler, 0);
  isr_init(&idt[INT_PAGE_FAULT], (uint32_t)irq14_handler, 0);
  isr_init(&idt[SYSCALL], (uint32_t)irq128_handler, 3);
  
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
