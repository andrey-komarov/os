ENTRY(_start)

SECTIONS
{
        . = 0x14880000;
        .text : { *(.text) }
        .data : { *(.data) }
        .rodata : { *(.rodata) }
        .bss : { *(.bss) }
}
