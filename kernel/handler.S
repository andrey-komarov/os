
.section .text
.global interrupt_handler
.global irq0_handler
.global irq1_handler
.extern printk
.extern pic_send_eoi

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
        pushl $0
        call pic_send_eoi
        addl $4, %esp
        iret
        
# kbd
irq1_handler:
        xor %eax, %eax
        inb $0x60, %al
        pushl %eax
        pushl $keypressed
        call printk
        addl $8, %esp
        pushl $0
        call pic_send_eoi
        addl $4, %esp
        iret
 

.section .data

x:      .long 0
        
hello:  .asciz "Interrupt #%d esp=%p!"
keypressed:      .asciz "Key pressed: %p!"
timer:      .asciz "Hello timer"
