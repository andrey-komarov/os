	.section .text

	.extern runirq
	.extern runint
	
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


	MKIRQHANDLER 0		# timer
	MKIRQHANDLER 1		# kbd
	MKIRQHANDLER 14		# ata1
	MKIRQHANDLER 15		# ata2

	MKINTHANDLER 8		# double fault
	MKINTHANDLER 13		# GPF
	MKINTHANDLER 14		# page fault
	MKINTHANDLER 42		# <unknown int>
	MKINTHANDLER 0x80	# syscall
