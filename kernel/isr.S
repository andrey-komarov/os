	.section .text

	.extern runirq
	.extern runint
	.extern runinterr
	
	.macro MKIRQHANDLER num
	.global irq\num\()handler
irq\num\()handler :
	pushal
	pushl %ds
	pushl %es
	pushl %ebp

	pushl $\num
	call runirq
	addl $4, %esp

	popl %ebp
	popl %es
	popl %ds
	popal
	iret
	.endm

	.macro MKINTHANDLER num
	.global int\num\()handler
int\num\()handler :
	pushal
	pushl %ds
	pushl %es
	pushl %ebp

	pushl $\num
	call runint
	addl $4, %esp

	popl %ebp
	popl %es
	popl %ds
	popal
	iret
	.endm

	.macro MKINTERRHANDLER num
	.global interr\num\()handler
interr\num\()handler :
	pushal
	movl 32(%esp), %eax 	# error code (or 32?)
	pushl %ds
	pushl %es
	pushl %ebp

	pushl %eax
	pushl $\num
	call runinterr
	addl $8, %esp

	popl %ebp
	popl %es
	popl %ds
	popal
	addl $4, %esp 		# pop error code
	iret
	.endm

	MKIRQHANDLER 0		# timer
	MKIRQHANDLER 1		# kbd
	MKIRQHANDLER 14		# ata1
	MKIRQHANDLER 15		# ata2

	MKINTERRHANDLER 8		# double fault
	MKINTERRHANDLER 13		# GPF
	MKINTERRHANDLER 14		# page fault
	MKINTHANDLER 42		# <unknown int>
	MKINTHANDLER 0x80	# syscall
