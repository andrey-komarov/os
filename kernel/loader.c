#include "kernel/loader.h"
#include "kernel/elf32.h"

#include "fs/fat16.h"
#include "libc/panic.h"
#include "libc/assert.h"
#include "mem/kpmalloc.h"
#include "kernel/kernel.h"
#include "libc/string.h"
#include "kernel/userspace.h"

#define STACK_SIZE (4096 * 16)

void load_elf_from_file(char *path, pagedir_t *ctx)
{
  fat16_fd_t fd;
  if (fat16_open(path, &fd) == NULL)
    panic("Cannot open file");
  size_t pages = (fd.file_size + PAGE_SIZE - 1) / PAGE_SIZE;
  uint8_t *data = kpmalloc(pages);
  if ((size_t)fat16_read(&fd, data, 0, fd.file_size) != fd.file_size)
    panic("Failed to read file");
  uint8_t sum = 0;
  for (size_t i = 0; i < fd.file_size; i++)
    sum += data[i];
  printk("sum = %d", sum);
  load_elf(data, fd.file_size, ctx);
  kpfree(data, pages);
}

static void map_page(void *page, pagedir_t *ctx)
{
  uint32_t p = (uint32_t)page;
  assert(p % PAGE_SIZE == 0);
  size_t ptno = p / PAGE_SIZE / PAGE_TABLE_SIZE;
  size_t pageno = p / PAGE_SIZE % PAGE_TABLE_SIZE;
  uint32_t pdentry = (*ctx)[ptno];
  if (!(pdentry & PD_PRESENT))
    {
      uint32_t phy_newtable = (uint32_t)phymem_alloc_page();
      uint8_t buf[PAGE_SIZE];
      memset(buf, 0, sizeof(buf));
      write_phy((void*)phy_newtable, buf, sizeof(buf));
      pdentry = phy_newtable | PD_PRESENT | PD_RW | PD_USER;
      (*ctx)[ptno] = pdentry;
    }
  uint32_t *phy_table = (uint32_t*)(pdentry & PD_ADDR);
  uint32_t ptentry;
  read_phy(&ptentry, &phy_table[pageno], sizeof(ptentry));
  if (!(ptentry & PT_PRESENT))
    {
      uint32_t phy_newpage = (uint32_t)phymem_alloc_page();
      ptentry = phy_newpage | PT_PRESENT | PT_RW | PT_USER;
      write_phy(&phy_table[pageno], &ptentry, sizeof(ptentry));
    }
}

static void map_range(void *start, size_t size, pagedir_t *ctx)
{
  uint32_t p = (uint32_t)start;
  assert(p % PAGE_SIZE == 0);
  assert(size % PAGE_SIZE == 0);
  for (size_t i = 0; i < size / PAGE_SIZE; i++)
    map_page((void*)(p + i * PAGE_SIZE), ctx);
}

static void map_pheader(Elf32_Ehdr *hdr, Elf32_Phdr *phdr, pagedir_t *ctx)
{
  assert(phdr->p_type == PT_LOAD);
  assert(phdr->p_align == PAGE_SIZE);
  void *addr = (void*)(phdr->p_vaddr & ~(PAGE_SIZE - 1));
  uint32_t size = ((((char*)phdr->p_vaddr) + phdr->p_memsz - (char*)addr - 1) / PAGE_SIZE + 1) * PAGE_SIZE;
  map_range(addr, size, ctx);
  *((int*)addr) = 1488;
  memcpy((char*)phdr->p_vaddr, ((char*)hdr) + phdr->p_offset, phdr->p_filesz);
  memset((char*)phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
}

static void *verify_elf_header(Elf32_Ehdr *hdr, pagedir_t *ctx)
{
  assert(hdr->e_ident[EI_MAG0] == ELFMAG0);
  assert(hdr->e_ident[EI_MAG1] == ELFMAG1);
  assert(hdr->e_ident[EI_MAG2] == ELFMAG2);
  assert(hdr->e_ident[EI_MAG3] == ELFMAG3);
  assert(hdr->e_ident[EI_CLASS] == ELFCLASS32);
  assert(hdr->e_ident[EI_DATA] == ELFDATA2LSB);
  assert(hdr->e_ident[EI_VERSION] == EV_CURRENT);
  //assert(hdr->e_ident[EI_OSABI] == ELFOSABI_SYSV);
  assert(hdr->e_ident[EI_ABIVERSION] == 0);
  for (int i = EI_PAD; i < EI_NIDENT; i++)
    assert(hdr->e_ident[i] == 0);
  assert(hdr->e_type == ET_EXEC);
  assert(hdr->e_machine == EM_386);
  printk("entry point = %p", hdr->e_entry);
  printk("program header table off = %p", hdr->e_phoff);
  printk("section header table off = %p", hdr->e_shoff);
  assert(hdr->e_flags == 0);
  assert(hdr->e_ehsize == sizeof(*hdr));
  printk("phentsize = %d, phnum = %d", hdr->e_phentsize, hdr->e_phnum);
  printk("shentsize = %d, shnum = %d", hdr->e_shentsize, hdr->e_shnum);
  printk("strings table index = %d", hdr->e_shstrndx);
  
  assert(hdr->e_phentsize == sizeof(Elf32_Phdr));
  Elf32_Phdr *pheaders = (Elf32_Phdr*)((char*)hdr + hdr->e_phoff);
  printk("=============================");
  printk("Checking headers");
  for (int i = 0; i < hdr->e_phnum; i++)
    {
      switch (pheaders[i].p_type)
        {
        case PT_LOAD:
          printk("%d is loadable", i);
          //assert(headers[i].p_paddr == 0);
          printk("  p_paddr = %p", pheaders[i].p_paddr);
          printk("  loading info: from %p, %d bytes from file, %d bytes total",
                 pheaders[i].p_vaddr, pheaders[i].p_filesz, pheaders[i].p_memsz);
          printk("  %d alignment, %p offset, %s%s%s",
                 pheaders[i].p_align,
                 pheaders[i].p_offset,
                 pheaders[i].p_flags & PF_X ? "x" : "-",
                 pheaders[i].p_flags & PF_W ? "w" : "-",
                 pheaders[i].p_flags & PF_R ? "r" : "-");
	  map_pheader(hdr, &pheaders[i], ctx);
	  break;
        default:
          printk("%d has unknown type: %d", i, pheaders[i].p_type);
        }
    }
  
  assert(hdr->e_shentsize == sizeof(Elf32_Shdr));
  Elf32_Shdr *sheaders = (Elf32_Shdr*)((char*)hdr + hdr->e_shoff);
  printk("=============================");
  printk("Checking sections");
  for (int i = 0; i < hdr->e_shnum; i++)
    {
      printk("%d: %d %d %p", i, sheaders[i].sh_name, sheaders[i].sh_type, sheaders[i].sh_flags);
    }
  
  return (void*)hdr->e_entry;

}

static void *prepare_stack(int argc, char **argv, pagedir_t *ctx)
{
  void *stack = (char*)(KERNEL_VMA) - STACK_SIZE;
  printk("prepare %p %p", stack, STACK_SIZE);
  map_range(stack, STACK_SIZE, ctx);
  int skip = 0;
  for (int i = 0; i < argc; i++)
    skip += strlen(argv[i]) + 1;
  char *top = (char*)stack + STACK_SIZE;
  top -= 4;
  *(uint32_t*)top = 0;
  printk("lol");
  char *top2 = top - skip;
  top2 -= 4;
  *(uint32_t*)top2 = 0;
  for (int i = argc - 1; i >= 0; i--)
    {
      int len = strlen(argv[i]);
      memcpy(top - len - 1, argv[i], len + 1);
      top -= len + 1;
      top2 -= 4;
      *(char**)top2 = top;
    }
  top2 -= 4;
  *(uint32_t*)top2 = argc;
  return top2;
}

void load_elf(uint8_t *elf, size_t len, pagedir_t *ctx)
{
  void *eip = verify_elf_header((Elf32_Ehdr*)elf, ctx);
  char *argv[] = {"lol", NULL};
  size_t argc = sizeof(argv) / sizeof(argv[0]) - 1;
  printk("preparing stack...");
  void *esp = prepare_stack(argc, argv, ctx);
  printk("Entry point: %p %p", eip, esp);
  jmp_to_userspace(eip, esp);
}
