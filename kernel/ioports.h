#ifndef _IOPORTS_H
#define _IOPORTS_H

#include <stdint.h>

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
void io_wait();

#endif
