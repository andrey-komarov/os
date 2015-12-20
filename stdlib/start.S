.section .text
.extern main
.extern _exit
.global _start

_start:
        movl (%esp), %eax
        leal 0x4(%esp), %ebx
        pushl %ebx
        pushl %eax
        call main
        pushl %eax
        call _exit

