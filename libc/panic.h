#ifndef _PANIC_H
#define _PANIC_H

void run_panic(char* s) __attribute__((noreturn));

#define panic(s) \
  run_panic("Kernel panic at file " __FILE__ ": "#s)

#endif // _PANIC_H
