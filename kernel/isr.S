	.section .text

	.extern runirq
	
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

	MKIRQHANDLER 0
	MKIRQHANDLER 1
	MKIRQHANDLER 14
	MKIRQHANDLER 15
	MKIRQHANDLER 42
