#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>

#define IDT_SIZE 256
#define INT_DOUBLE_FAULT 8
#define INT_GPF 13
#define INT_PAGE_FAULT 14

typedef struct
{
   uint16_t offset_1;
   uint16_t selector;
   uint8_t zero;
   uint8_t type_attr;
   uint16_t offset_2;
} idt_entry_t;

void init_interrupts();

void enable_interrupts();

void disable_interrupts();

void interrupt_handler();
void irq0_handler();
void irq1_handler();
void irq8_handler();
void irq13_handler();
void irq14_handler();

#endif // _INTERRUPTS_H
