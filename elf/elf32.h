#ifndef _ELF_H
#define _ELF_H

#include <stdint.h>

// from `man elf`

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Section;
typedef uint16_t Elf32_Versym;
typedef unsigned char Elf_Byte;
typedef uint16_t Elf32_Half;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;
typedef int64_t Elf32_Sxword;
typedef uint64_t Elf32_Xword;

#define EI_NIDENT 16

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  uint16_t      e_type;
  uint16_t      e_machine;
  uint32_t      e_version;
  Elf32_Addr     e_entry;
  Elf32_Off      e_phoff;
  Elf32_Off      e_shoff;
  uint32_t      e_flags;
  uint16_t      e_ehsize;
  uint16_t      e_phentsize;
  uint16_t      e_phnum;
  uint16_t      e_shentsize;
  uint16_t      e_shnum;
  uint16_t      e_shstrndx;
} Elf32_Ehdr;

#define EI_MAG0 0
#define ELFMAG0 0x7f
#define EI_MAG1 1
#define ELFMAG1 'E'
#define EI_MAG2 2
#define ELFMAG2 'L'
#define EI_MAG3 3
#define ELFMAG3 'F'

#define EI_CLASS 4
#define ELFCLASS32 1

#define EI_DATA 5
#define ELFDATA2LSB 1

#define EI_VERSION 6
#define EV_CURRENT 1

#define EI_OSABI 7
#define ELFOSABI_SYSV 0

#define EI_ABIVERSION 8

#define EI_PAD 9


#define ET_EXEC 2

#define EM_386 3

typedef struct {
  uint32_t   p_type;
  Elf32_Off  p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  uint32_t   p_filesz;
  uint32_t   p_memsz;
  uint32_t   p_flags;
  uint32_t   p_align;
} Elf32_Phdr;

#define PT_LOAD 1

#define PF_X 1
#define PF_W 2
#define PF_R 4

typedef struct {
  uint32_t   sh_name;
  uint32_t   sh_type;
  uint32_t   sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off  sh_offset;
  uint32_t   sh_size;
  uint32_t   sh_link;
  uint32_t   sh_info;
  uint32_t   sh_addralign;
  uint32_t   sh_entsize;
} Elf32_Shdr;

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_NOBITS 8


#endif // _ELF_H
