#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>

#define IDT_SIZE 256

struct idt_entry_t
{
   uint16_t offset_1;
   uint16_t selector;
   uint8_t zero;
   uint8_t type_attr;
   uint16_t offset_2;
};

void init_interrupts();

void enable_interrupts();

void disable_interrupts();

#endif // _INTERRUPTS_H
