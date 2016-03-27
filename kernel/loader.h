#ifndef _LOADER_C
#define _LOADER_C

#include <stdint.h>
#include <stddef.h>

#include "kernel/mmu.h"

void load_elf_from_file(char *filename, pagedir_t *ctx);
void load_elf(uint8_t *elf, size_t len, pagedir_t *ctx);

#endif // _LOADER_C
