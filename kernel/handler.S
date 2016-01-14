
.section .text
.global interrupt_handler
.global pic0_handler
.global pic1_handler
.global pic14_handler
.global pic15_handler
.global irq8_handler
.global irq13_handler
.global irq14_handler
.global irq128_handler
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
pic0_handler:
#        pushl $timer
#        call printk
#        addl $4, %esp
        pushl $0
        call pic_send_eoi
        addl $4, %esp
        iret
        
# kbd
pic1_handler:
        xor %eax, %eax
        inb $0x60, %al
        pushl %eax
        pushl $keypressed
        call printk
        addl $8, %esp
        pushl $1
        call pic_send_eoi
        addl $4, %esp
        iret

# primary ATA
pic14_handler:
        xor %eax, %eax
        inb $0x3f6, %al
        pushl %eax
        pushl $ata1
        call printk
        addl $8, %esp
        pushl $14
        call pic_send_eoi
        addl $4, %esp
        iret

# secondary ATA
pic15_handler:
        pushl $ata2
        call printk
        addl $4, %esp
        pushl $15
        call pic_send_eoi
        addl $4, %esp
        iret
 
# double fault
irq8_handler:
        pushl $doublefault
        call printk
        addl $4, %esp
        iret

# gpf
irq13_handler:
        pushl $gpf
        call printk
        addl $4, %esp
        iret

# page fault
irq14_handler:
        pushl $pagefault
        call printk
        addl $4, %esp
        iret

# syscall
irq128_handler:
        pushl $syscall
        call printk
        addl $4, %esp
        iret
.section .data

x:      .long 0
        
hello:  .asciz "Interrupt #%d esp=%p!"
keypressed:      .asciz "Key pressed: %p!"
timer:      .asciz "Hello timer"
gpf:    .asciz "General Protection Failed"
pagefault:      .asciz "Page Fault"
doublefault:      .asciz "Page Fault"
syscall:        .asciz "Syscall!"
ata1:   .asciz "Primary ATA interrupt, status=%x"
ata2:   .asciz "Secondary ATA interrupt"

