#include "interrupts.h"
#include "gdt.h"
#include "kernel.h"
#include "pic.h"

static idt_entry_t idt[IDT_SIZE];

void runirq(uint32_t irq)
{
  printk("irq %d received", irq);
  pic_send_eoi(irq);
}

void set_idt(uint16_t size, idt_entry_t* addr)
{
  struct {
    uint16_t size;
    uint32_t addr;
  } __attribute__((packed)) gdtr = {.size = size - 1, .addr = (uint32_t)addr};

  __asm volatile("lidt (%0)": : "a"(&gdtr));
  printk("IDT set");
}

static void isr_init(uint32_t idtno, uint32_t addr, int dpl)
{
  idt_entry_t *cur_idt = &idt[idtno];
  cur_idt->offset_1 = ((uint32_t)addr) & 0xffff;
  cur_idt->selector = KERNEL_CODE_SEGMENT * 8;
  cur_idt->zero = 0;
  // 0x80 - present
  // type = 32-bit interrupt gate
  cur_idt->type_attr = 0x8e | (dpl << 5);
  cur_idt->offset_2 = (((uint32_t)addr) >> 16) & 0xffff;
}

void init_interrupts()
{
  for (int i = 0; i < IDT_SIZE; i++)
    {
      isr_init(i, (uint32_t)interrupt_handler, 0);
    }
	
  isr_init(PIC1 + PIC_TIMER, irq0handler, 0);
  isr_init(PIC1 + PIC_KBD, irq1handler, 0);
  isr_init(PIC2 + PIC_ATA1, irq14handler, 0);
  isr_init(PIC2 + PIC_ATA2, irq15handler, 0);
  isr_init(INT_DOUBLE_FAULT, (uint32_t)irq8_handler, 0);
  isr_init(INT_GPF, (uint32_t)irq13_handler, 0);
  isr_init(INT_PAGE_FAULT, (uint32_t)irq14_handler, 0);
  isr_init(SYSCALL, (uint32_t)irq128_handler, 3);
  
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
