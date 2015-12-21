#ifndef _UNISTD_H_MY
#define _UNISTD_H_MY

#include <stdint.h>
#include <stddef.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

void _exit(int status) __attribute__((noreturn));

int32_t write(int fd, const void *buf, size_t count);

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

int open(const char *pathname, int flags);

void *mmap(void *addr, size_t length, int prot,
            int flags, int fd, int offset);

#endif // _UNISTD_H
