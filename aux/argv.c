#include "unistd.h"
#include "string.h"
#include "stdio.h"

const char delim[] = "========================================\n";

int main(int argc, char** argv)
{
  printf("%s", delim);
  //  write(1, delim, sizeof(delim));
  for (int i = 0; i < argc; i++)
    {
      write(1, argv[i], strlen(argv[i]));
      write(1, "\n", 1);
      write(1, delim, sizeof(delim));
    }
  return 3;
}
