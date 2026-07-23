/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 QEMU virt entry scaffold (not linked into product x86_64 build).
 * Expects firmware/UEFI or QEMU -kernel to enter at _start in EL1.
 */
.section .text.boot
.global _start
.global aarch64_kmain

_start:
    /* Disable interrupts until GIC/VBAR ready */
    msr daifset, #0xf
    /* Stack from linker */
    adrp x0, __stack_top
    add  x0, x0, :lo12:__stack_top
    mov  sp, x0
    bl   aarch64_kmain
1:
    wfe
    b    1b
