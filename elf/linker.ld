ENTRY(_start)

SECTIONS
{
        . = 0x08048000;
        .text : { *(.text) }
        .data : { *(.data) }
        .rodata : { *(.rodata) }
        .bss : { *(.bss) }

        PROVIDE (__preinit_array_start = .);
        .preinit_array     : { *(.preinit_array) }
        PROVIDE (__preinit_array_end = .);
        PROVIDE (__init_array_start = .);
        .init_array     : { *(.init_array) }
        PROVIDE (__init_array_end = .);
        PROVIDE (__fini_array_start = .);
        .fini_array     : { *(.fini_array) }
        PROVIDE (__fini_array_end = .);

}
