#include "unistd.h"
#include "syscall.h"

void _exit(int status)
{
  syscall1(SYS_EXIT, (uint32_t)status);
  __builtin_unreachable();
}

int32_t write(int fd, const void *buf, size_t count)
{
  return (int32_t)syscall3(SYS_WRITE, (uint32_t)fd, (uint32_t)buf, (uint32_t)count);
}
