#include "gdt.h"
#include "tss.h"

void gdt_entry_init(gdt_entry_t* entry)
{
  entry->limit_1 = 0;
  entry->base_1 = 0;
  entry->base_2 = 0;
  entry->access = 1 << 4;
  entry->flags = 0;
  entry->base_3 = 0;
}

void gdt_set_limit(gdt_entry_t* entry, uint32_t limit)
{
  entry->limit_1 = limit & 0xffff;
  entry->flags = (entry->flags & 0xf0) | ((limit >> 16) & 0xf);
}

uint32_t gdt_get_limit(gdt_entry_t* entry)
{
  uint32_t limit_1 = entry->limit_1;
  uint32_t limit_2 = entry->flags & 0xf;
  return limit_1 | (limit_2 << 16);
}

void gdt_set_base(gdt_entry_t* entry, uint32_t base)
{
  entry->base_1 = base & 0xffff;
  entry->base_2 = (base >> 16) & 0xff;
  entry->base_3 = (base >> 24) & 0xff;
}

uint32_t gdt_get_base(gdt_entry_t* entry)
{
  uint32_t base_1 = entry->base_1;
  uint32_t base_2 = entry->base_2;
  uint32_t base_3 = entry->base_3;
  return base_1 | (base_2 << 16) | (base_3 << 24);
}

void gdt_set_priv(gdt_entry_t* entry, uint8_t priv)
{
  entry->access = (entry->access & (~GDT_ACCESS_PRIV)) | (priv << 5);
}

uint8_t gdt_get_priv(gdt_entry_t* entry)
{
  return (entry->access & GDT_ACCESS_PRIV) >> 5;
}

gdt_entry_t gdt[GDT_SIZE];

static void reload_segments()
{
  __asm volatile(
    "ljmp %0, $.reload_cs\n\t"
    ".reload_cs:\n\t"
    "movw %1, %%ax\n\t"
    "movw %%ax, %%ds\n\t"
    "movw %%ax, %%es\n\t"
    "movw %%ax, %%fs\n\t"
    "movw %%ax, %%gs\n\t"
    "movw %%ax, %%ss\n\t"
    :
    : "i"(KERNEL_CODE_SEGMENT * sizeof(gdt_entry_t)), "i"(KERNEL_DATA_SEGMENT * sizeof(gdt_entry_t))
    : "ax");
}

void set_gdtr(uint16_t size, gdt_entry_t* addr)
{
  struct {
    uint16_t size;
    uint32_t addr;
  } __attribute__((packed)) gdtr = {.size = size - 1, .addr = (uint32_t)addr};

  __asm volatile("lgdt (%0)": : "a"(&gdtr));
  reload_segments();
}

void init_gdt()
{
  for (int i = 0; i < GDT_SIZE; i++)
    gdt_entry_init(&gdt[i]);
  
  gdt_set_base(&gdt[EMPTY_SEGMENT], 0);
  gdt_set_limit(&gdt[EMPTY_SEGMENT], 0);

  gdt_set_base(&gdt[KERNEL_CODE_SEGMENT], 0);
  gdt_set_limit(&gdt[KERNEL_CODE_SEGMENT], 0xfffff);
  gdt_set_priv(&gdt[KERNEL_CODE_SEGMENT], 0);
  gdt[KERNEL_CODE_SEGMENT].access |= 
    GDT_ACCESS_PR | GDT_ACCESS_EX | GDT_ACCESS_RW;
  gdt[KERNEL_CODE_SEGMENT].flags |=
    GDT_FLAGS_GR | GDT_FLAGS_SZ;

  gdt_set_base(&gdt[KERNEL_DATA_SEGMENT], 0);
  gdt_set_limit(&gdt[KERNEL_DATA_SEGMENT], 0xfffff);
  gdt_set_priv(&gdt[KERNEL_DATA_SEGMENT], 0);
  gdt[KERNEL_DATA_SEGMENT].access |=
    GDT_ACCESS_PR | GDT_ACCESS_RW;
  gdt[KERNEL_DATA_SEGMENT].flags |=
    GDT_FLAGS_GR | GDT_FLAGS_SZ;
  
  gdt_set_base(&gdt[USER_CODE_SEGMENT], 0);
  gdt_set_limit(&gdt[USER_CODE_SEGMENT], 0xfffff);
  gdt_set_priv(&gdt[USER_CODE_SEGMENT], 3);
  gdt[USER_CODE_SEGMENT].access |=
    GDT_ACCESS_PR | GDT_ACCESS_EX | GDT_ACCESS_RW;
  gdt[USER_CODE_SEGMENT].flags |=
    GDT_FLAGS_GR | GDT_FLAGS_SZ;

  gdt_set_base(&gdt[USER_DATA_SEGMENT], 0);
  gdt_set_limit(&gdt[USER_DATA_SEGMENT], 0xfffff);
  gdt_set_priv(&gdt[USER_DATA_SEGMENT], 3);
  gdt[USER_DATA_SEGMENT].access |=
    GDT_ACCESS_PR | GDT_ACCESS_RW;
  gdt[USER_DATA_SEGMENT].flags |=
    GDT_FLAGS_GR | GDT_FLAGS_SZ;
  
  gdt_set_base(&gdt[TSS_SEGMENT], (uint32_t)&tss_entry);
  gdt_set_limit(&gdt[TSS_SEGMENT], sizeof(tss_entry));
  gdt_set_priv(&gdt[TSS_SEGMENT], 3);
  gdt[TSS_SEGMENT].access =
    GDT_ACCESS_PR | GDT_ACCESS_EX | GDT_ACCESS_AC;
  gdt[TSS_SEGMENT].flags |=
    GDT_FLAGS_SZ;
  
  set_gdtr(sizeof(gdt), gdt);
  
  tss_entry.ss0 = KERNEL_DATA_SEGMENT * sizeof(gdt_entry_t);
  
  tss_flush();
}
