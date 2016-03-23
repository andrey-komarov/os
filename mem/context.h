#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "kernel/mmu.h"

pagedir_t *context_new();
void context_init_kernel_pages(pagedir_t *context);
void context_free(pagedir_t *context);

#endif // _CONTEXT_H
