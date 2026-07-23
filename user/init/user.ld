/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/*
 * Freestanding userspace ELF linker script (shared by init, shell, servers).
 * Base above kernel BSS (see GJ_USER_CODE_VA). Parent Makefile owns the
 * link line; this tree only ships the script.
 */
ENTRY(_start)

SECTIONS
{
    . = 0x1000000;

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
        *(.bss .bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.note .note.*)
        *(.comment)
        *(.eh_frame*)
    }
}
