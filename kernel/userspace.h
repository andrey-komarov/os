#ifndef _USERSPACE_H
#define _USERSPACE_H

void test_userspace() __attribute__((noreturn));

void jmp_to_userspace(void *eip, void *esp);

#endif // _USERSPACE_H
