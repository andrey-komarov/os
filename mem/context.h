#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "kernel/mmu.h"

pagedir_t *context_new();
void context_clone_present(pagedir_t *dst, pagedir_t *src);
void context_free(pagedir_t *context);

#endif // _CONTEXT_H
