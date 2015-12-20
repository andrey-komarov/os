	.section .text
	.global _start
	
_start:
	movl $4, %eax
        movl $1, %ebx
        movl $hello, %ecx
        movl $SZ, %edx
        int $0x80

        movl $1, %eax
        xorl %ebx, %ebx
        int $0x80

	.section .data
hello:  .asciz "Hello static binary!\n\0"
        SZ = . - hello
