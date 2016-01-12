#include "ioports.h"

uint8_t inb(uint16_t port)
{
  uint8_t result;
  __asm volatile("inb %1, %0"
        : "=al"(result)
        : "dx"(port)
        :);
  return result;
}

uint16_t inw(uint16_t port)
{
  uint16_t result;
  __asm volatile("inw %1, %0"
        : "=ax"(result)
        : "dx"(port)
        :);
  return result;
}

void outb(uint16_t port, uint8_t value)
{
  __asm volatile("outb %0, %1"
        :
        : "al"(value), "dx"(port)
        :);
}

void io_wait()
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    __asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    /* TODO: Is there any reason why al is forced? */
}

