#ifndef _KPMALLOC_H
#define _KPMALLOC_H

#include <stddef.h>

void init_vmem();

void* kpmalloc(size_t pages);
void kpfree(void *addr, size_t pages);

#endif // _KPMALLOC_H
