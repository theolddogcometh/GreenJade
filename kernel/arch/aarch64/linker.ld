/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 QEMU virt linker script (product scaffold).
 * Load at DRAM base 0x4000_0000 (QEMU -kernel EL1 identity).
 */
ENTRY(_start)

SECTIONS
{
    . = 0x40000000;
    .text : {
        *(.text.boot)
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
        . = ALIGN(16);
        . += 0x8000; /* early stack (32 KiB) */
        __stack_top = .;
        __bss_end = .;
    }
    . = ALIGN(4096);
    __kernel_end = .;
}
