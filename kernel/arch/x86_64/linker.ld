/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x86_64 Multiboot2 / long-mode kernel linker script (product scaffold).
 * Load at 1 MiB; .boot holds Multiboot2 header.
 */
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
