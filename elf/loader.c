#include "elf32.h"

#include "unistd.h"
#include "stdio.h"

#define LOAD_ADDR 0x66661000

int main(int argc, char **argv)
{
  if (argc != 2)
    {
      printf("usage: %s <elf file>\n", argv[0]);
      _exit(EXIT_FAILURE);
    }
  
  
  return 0;
}
