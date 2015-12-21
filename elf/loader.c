#include "elf32.h"

#include "unistd.h"
#include "stdio.h"
#include "sys/mman.h"
#include "assert.h"

void verify_elf_header(Elf32_Ehdr *hdr)
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
          printf("  %d alignment, %s%s%s\n",
                 pheaders[i].p_align,
                 pheaders[i].p_flags & PF_X ? "x" : "-",
                 pheaders[i].p_flags & PF_W ? "w" : "-",
                 pheaders[i].p_flags & PF_R ? "r" : "-");
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
      printf("%d: %d %d\n", i, sheaders[i].sh_name, sheaders[i].sh_type);
    }
}

int main(int argc, char **argv)
{
  if (argc != 2)
    {
      printf("usage: %s <elf file>\n", argv[0]);
      _exit(EXIT_FAILURE);
    }
  
  int fd = open(argv[1], O_RDONLY);
  if (fd < 0)
    {
      printf("failed to open <%s>", argv[1]);
      _exit(EXIT_FAILURE);
    }
  
  void *m = mmap2(NULL, 20000, PROT_READ, MAP_PRIVATE, fd, 0);
  printf("m = %p\n", m);
  
  verify_elf_header(m);
  
  return 0;
}
