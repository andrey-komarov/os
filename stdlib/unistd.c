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

int open(const char *path, int flags)
{
  return (int)syscall2(SYS_OPEN, (uint32_t)path, (uint32_t)flags);
}

void *mmap2(void *addr, size_t length, int prot,
            int flags, int fd, int pgoffset)
{
  struct {
    uint32_t addr;
    uint32_t length;
    uint32_t prot;
    uint32_t flags;
    uint32_t fd;
    uint32_t pgoffset;
  } mm = {
    .addr = (uint32_t)addr,
    .length = length,
    .prot = prot,
    .flags = flags,
    .fd = fd,
    .pgoffset = pgoffset
  };
  return (void*)syscall1(SYS_MMAP, (uint32_t)&mm);
}
