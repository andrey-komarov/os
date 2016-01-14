#ifndef _PANIC_H
#define _PANIC_H

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

void run_panic(char* s) __attribute__((noreturn));

#define panic(s) \
  run_panic("Kernel panic at file " __FILE__ " at line " STR(__LINE__) ": "#s)

#endif // _PANIC_H
