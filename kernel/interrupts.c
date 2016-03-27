#include "interrupts.h"
#include "gdt.h"
#include "kernel.h"
#include "pic.h"
#include "libc/panic.h"
#include "kernel/handlers.h"

static idt_entry_t idt[IDT_SIZE];

void (*irq_handlers[16])();
void (*int_handlers[256])();
void (*err_handlers[16])(uint32_t err);

void runirq(uint32_t irq)
{
  if (irq_handlers[irq])
    irq_handlers[irq]();
  else
    panic("Unknown IRQ");
  pic_send_eoi(irq);
}

void runint(uint32_t intr)
{
  if (int_handlers[intr])
    int_handlers[intr]();
  else
    panic("Unknown int");
  printk("int %d", intr);
}

void runinterr(uint32_t intr, uint32_t err)
{
  if (err_handlers[intr])
    err_handlers[intr](err);
  else
    panic("Unknown err");
  printk("err %d %d", intr, err);
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

static void isr_init(uint32_t idtno, void (*addr)(), int dpl)
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
      isr_init(i, (uint32_t)int42handler, 0);
    }
	
  isr_init(PIC1 + PIC_TIMER, irq0handler, 0);
  irq_handlers[PIC_TIMER] = irq_timer;
  isr_init(PIC1 + PIC_KBD, irq1handler, 0);
  irq_handlers[PIC_KBD] = irq_kbd;
  isr_init(PIC2 + PIC_ATA1, irq14handler, 0);
  irq_handlers[PIC_ATA1 + 8] = irq_ata1;
  isr_init(PIC2 + PIC_ATA2, irq15handler, 0);
  irq_handlers[PIC_ATA2 + 8] = irq_ata2;

  isr_init(INT_DOUBLE_FAULT, interr8handler, 0);
  err_handlers[INT_DOUBLE_FAULT] = int_df;
  isr_init(INT_GPF, interr13handler, 0);
  err_handlers[INT_GPF] = int_gpf;
  isr_init(INT_PAGE_FAULT, interr14handler, 0);
  err_handlers[INT_PAGE_FAULT] = int_page_fault;
  isr_init(SYSCALL, int0x80handler, 3);
  int_handlers[SYSCALL] = int_syscall;
  
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
