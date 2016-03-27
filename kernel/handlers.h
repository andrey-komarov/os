#ifndef _HANDLERS_H
#define _HANDLERS_H

void irq_timer();
void irq_kbd();
void irq_ata1();
void irq_ata2();

void int_gpf();
void int_df();
void int_syscall();
void int_page_fault();

#endif // _HANDLERS_H
