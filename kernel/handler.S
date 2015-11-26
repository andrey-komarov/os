
.section .text
.global interrupt_handler
.global irq0_handler
.global irq1_handler
.extern printk

interrupt_handler:
        pushl %esp
        pushl x
        pushl $hello
        addl $1, x
        call printk
        addl $12, %esp
        iret

# timer
irq0_handler:
        pushl $timer
        call printk
        addl $4, %esp
        iret
        
# kbd
irq1_handler:
        pushl $keypressed
        call printk
        addl $4, %esp
        iret
 

.section .data

x:      .long 0
        
hello:  .asciz "Interrupt #%d esp=%p!"
keypressed:      .asciz "Key pressed!"
timer:      .asciz "Hello timer"
