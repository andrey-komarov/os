
.section .text
.global isr
.extern printk
isr:
        
hello:  .asciz "Interrupt!"
