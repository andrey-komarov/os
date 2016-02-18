# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.global bootp_page_dir
.type _start, @function
_start:
        cli
        mov $bootp_page_dir, %ecx
        subl $0xc0000000, %ecx
        mov %ecx, %cr3
        mov %cr4, %ecx
        orl $0x00000010, %ecx
        mov %ecx, %cr4
        mov %cr0, %ecx
        orl $0x80000000, %ecx
        mov %ecx, %cr0

	movl $stack_top, %esp

        push $.Ljmp_to_highmem
        ret
.Ljmp_to_highmem:

        # Multiboot information structure
        pushl %ebx
        # magic
        pushl %eax
	call kernel_main

	cli
	hlt
.Lhang:
	jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
