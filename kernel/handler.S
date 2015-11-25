
.section .text
.global interrupt_handler
.extern printk

interrupt_handler:
        pushl %esp
        pushl x
        pushl $hello
        addl $1, x
        call printk
        addl $12, %esp
        iret

.section .data

x:      .long 0
        
hello:  .asciz "Interrupt #%d esp=%p!"
