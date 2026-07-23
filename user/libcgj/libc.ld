/* SPDX-License-Identifier: MIT OR Apache-2.0 */
/* Shared libc.so.6 preferred base (ET_DYN-friendly load). */
ENTRY(_libcgj_load)

SECTIONS
{
    . = 0x0;

    .hash : { *(.hash) }
    .gnu.hash : { *(.gnu.hash) }
    .dynsym : { *(.dynsym) }
    .dynstr : { *(.dynstr) }
    .rela.dyn : { *(.rela*) }

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
