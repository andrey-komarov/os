
.section .text
.global interrupt_handler
.extern printk

interrupt_handler:
        pushl x
        pushl $hello
        addl $1, x
        call printk
        addl $8, %esp
        ret

.section .data

x:      .long 0
        
hello:  .asciz "Interrupt #%d!"
