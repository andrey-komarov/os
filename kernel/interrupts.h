#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>

#define IDT_SIZE 256
#define INT_DOUBLE_FAULT 8
#define INT_GPF 13
#define INT_PAGE_FAULT 14
#define SYSCALL 0x80

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

void irq0handler();
void irq1handler();
void irq14handler();
void irq15handler();

void int8handler();
void int13handler();
void int14handler();
void int42handler();
void int0x80handler();

#endif // _INTERRUPTS_H
