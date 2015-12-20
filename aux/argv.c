#include "unistd.h"

const char delim[] = "========================================\n";

int strlen(char* str)
{
  int i;
  for (i = 0; str[i]; i++)
    ;
  return i;
}

int main(int argc, char** argv)
{
  write(1, delim, sizeof(delim));
  for (int i = 0; i < argc; i++)
    {
      write(1, argv[i], strlen(argv[i]));
      write(1, "\n", 1);
      write(1, delim, sizeof(delim));
    }
  return 3;
}
