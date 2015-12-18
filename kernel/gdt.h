#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define GDT_SIZE 6
#define EMPTY_SEGMENT 0
#define KERNEL_CODE_SEGMENT 1
#define KERNEL_DATA_SEGMENT 2
#define USER_CODE_SEGMENT 3
#define USER_DATA_SEGMENT 4
#define TSS_SEGMENT 5

#define GDT_ACCESS_AC (1 << 0)
#define GDT_ACCESS_RW (1 << 1)
#define GDT_ACCESS_DC (1 << 2)
#define GDT_ACCESS_EX (1 << 3)
#define GDT_ACCESS_PRIV ((1 << 5) | (1 << 6))
#define GDT_ACCESS_PR (1 << 7)

#define GDT_FLAGS_SZ (1 << 6)
#define GDT_FLAGS_GR (1 << 7)

typedef struct gdt_entry
{
  uint16_t limit_1;
  uint16_t base_1;
  uint8_t base_2;
  uint8_t access;
  uint8_t flags;
  uint8_t base_3;
} gdt_entry_t;

void gdt_entry_init(gdt_entry_t* entry);

void gdt_set_limit(gdt_entry_t* entry, uint32_t limit);

uint32_t gdt_get_limit(gdt_entry_t* entry);

void gdt_set_base(gdt_entry_t* entry, uint32_t base);

uint32_t gdt_get_base(gdt_entry_t* entry);

void gdt_set_priv(gdt_entry_t* entry, uint8_t priv);

uint8_t gdt_get_priv(gdt_entry_t* entry);

void init_gdt();

#endif // _GDT_H
