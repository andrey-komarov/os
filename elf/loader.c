#include "elf32.h"

#include "unistd.h"
#include "stdio.h"
#include "sys/mman.h"

#define LOAD_ADDR 0x66661000

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
  
  void *m = mmap2(NULL, 100, PROT_READ, MAP_PRIVATE, fd, 0);
  printf("m = %p\n", m);
  printf("%s\n", (char*)m);
  
  return 0;
}
