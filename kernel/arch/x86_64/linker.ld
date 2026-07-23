/* SPDX-License-Identifier: MIT */
ENTRY(_start)

SECTIONS
{
    . = 1M;

    .boot : {
        KEEP(*(.multiboot2))
        *(.boot)
    }

    .text : {
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    __kernel_start = 1M;
    __kernel_end = .;

    /DISCARD/ : {
        *(.note .note.*)
        *(.comment)
        *(.eh_frame*)
    }
}
