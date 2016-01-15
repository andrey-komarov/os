#include <stdint.h>
#include "unistd.h"
#include "stdio.h"

int main()
{
  uint8_t sum = 0;
  uint8_t buf[1024];
  int r;
  while ((r = read(0, buf, sizeof(buf))))
    {
      for (int i = 0; i < r; i++)
        sum += buf[i];
    }
  printf("Checksum: %d\n", sum);
  return 0;
}
