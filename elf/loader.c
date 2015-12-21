#include "elf32.h"

#include "unistd.h"
#include "stdio.h"
#include "sys/mman.h"
#include "assert.h"
#include "string.h"

#define PAGE_SIZE 4096
#define STACK_SIZE (4096 * 16)

void __attribute__((noreturn)) jmp(void *eip, void *esp) 
{
  __asm volatile(
    "movl %%ebx, %%esp\n\t"
    "pushl %%eax\n\t"
    "ret\n\t"
    :
    : "eax"(eip), "ebx"(esp)
    :
                 );
  __builtin_unreachable();
}

void *prepare_stack(int argc, char **argv)
{
  void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  int skip = 0;
  for (int i = 0; i < argc; i++)
    skip += strlen(argv[i]) + 1;
  char *top = (char*)stack + STACK_SIZE;
  top -= 4;
  *(uint32_t*)top = 0;
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

void mmap_pheader(Elf32_Ehdr *hdr, Elf32_Phdr *phdr)
{
  assert(phdr->p_type == PT_LOAD);
  assert(phdr->p_align == PAGE_SIZE);
  int prot = PROT_WRITE;
  if (phdr->p_flags & PF_X)
    prot |= PROT_EXEC;
  if (phdr->p_flags & PF_W)
    prot |= PROT_WRITE;
  if (phdr->p_flags & PF_R)
    prot |= PROT_READ;
  void *addr = (void*)(phdr->p_vaddr & ~(PAGE_SIZE - 1));
  uint32_t size = ((((char*)phdr->p_vaddr) + phdr->p_memsz - (char*)addr - 1) / PAGE_SIZE + 1) * PAGE_SIZE;
  void *mem = mmap(addr, size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(mem == addr);
  memcpy((char*)phdr->p_vaddr, ((char*)hdr) + phdr->p_offset, phdr->p_filesz);
  memset((char*)phdr->p_vaddr + phdr->p_filesz, 0, phdr->p_memsz - phdr->p_filesz);
}

void *verify_elf_header(Elf32_Ehdr *hdr)
{
  assert(hdr->e_ident[EI_MAG0] == ELFMAG0);
  assert(hdr->e_ident[EI_MAG1] == ELFMAG1);
  assert(hdr->e_ident[EI_MAG2] == ELFMAG2);
  assert(hdr->e_ident[EI_MAG3] == ELFMAG3);
  assert(hdr->e_ident[EI_CLASS] == ELFCLASS32);
  assert(hdr->e_ident[EI_DATA] == ELFDATA2LSB);
  assert(hdr->e_ident[EI_VERSION] == EV_CURRENT);
  assert(hdr->e_ident[EI_OSABI] == ELFOSABI_SYSV);
  assert(hdr->e_ident[EI_ABIVERSION] == 0);
  for (int i = EI_PAD; i < EI_NIDENT; i++)
    assert(hdr->e_ident[i] == 0);
  assert(hdr->e_type == ET_EXEC);
  assert(hdr->e_machine == EM_386);
  printf("entry point = %p\n", hdr->e_entry);
  printf("program header table off = %p\n", hdr->e_phoff);
  printf("section header table off = %p\n", hdr->e_shoff);
  assert(hdr->e_flags == 0);
  assert(hdr->e_ehsize == sizeof(*hdr));
  printf("phentsize = %d, phnum = %d\n", hdr->e_phentsize, hdr->e_phnum);
  printf("shentsize = %d, shnum = %d\n", hdr->e_shentsize, hdr->e_shnum);
  printf("strings table index = %d\n", hdr->e_shstrndx);
  
  assert(hdr->e_phentsize == sizeof(Elf32_Phdr));
  Elf32_Phdr *pheaders = (Elf32_Phdr*)((char*)hdr + hdr->e_phoff);
  printf("=============================\n");
  printf("Checking headers\n");
  for (int i = 0; i < hdr->e_phnum; i++)
    {
      switch (pheaders[i].p_type)
        {
        case PT_LOAD:
          printf("%d is loadable\n", i);
          //assert(headers[i].p_paddr == 0);
          printf("  p_paddr = %p\n", pheaders[i].p_paddr);
          printf("  loading info: from %p, %d bytes from file, %d bytes total\n",
                 pheaders[i].p_vaddr, pheaders[i].p_filesz, pheaders[i].p_memsz);
          printf("  %d alignment, %p offset, %s%s%s\n",
                 pheaders[i].p_align,
                 pheaders[i].p_offset,
                 pheaders[i].p_flags & PF_X ? "x" : "-",
                 pheaders[i].p_flags & PF_W ? "w" : "-",
                 pheaders[i].p_flags & PF_R ? "r" : "-");
          mmap_pheader(hdr, &pheaders[i]);
          break;
        default:
          printf("%d has unknown type: %d\n", i, pheaders[i].p_type);
        }
    }
  
  assert(hdr->e_shentsize == sizeof(Elf32_Shdr));
  Elf32_Shdr *sheaders = (Elf32_Shdr*)((char*)hdr + hdr->e_shoff);
  printf("=============================\n");
  printf("Checking sections\n");
  for (int i = 0; i < hdr->e_shnum; i++)
    {
      printf("%d: %d %d %p\n", i, sheaders[i].sh_name, sheaders[i].sh_type, sheaders[i].sh_flags);
    }
  
  return (void*)hdr->e_entry;
}

int main(int argc, char **argv)
{
  if (argc < 2)
    {
      printf("usage: %s <elf file> <arg>*\n", argv[0]);
      _exit(EXIT_FAILURE);
    }
  
  int fd = open(argv[1], O_RDONLY);
  if (fd < 0)
    {
      printf("failed to open <%s>", argv[1]);
      _exit(EXIT_FAILURE);
    }
  
  void *m = mmap(NULL, 1<<20, PROT_READ, MAP_PRIVATE, fd, 0);
  printf("m = %p\n", m);
  
  void *eip = verify_elf_header(m);
  void *esp = prepare_stack(argc - 1, argv + 1);
  jmp(eip, esp);

  return 0;
}
