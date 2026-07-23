/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 QEMU virt entry scaffold (not linked into product x86_64 build).
 * Expects firmware/UEFI or QEMU -kernel to enter at _start in EL1.
 *
 * Soft deepen (careful; ABI stable):
 *   - DAIF fully masked until GIC/VBAR ready (product still holds I)
 *   - Soft CurrentEL check: only EL1 continues; other ELs WFE-hang
 *   - 16-byte SP align (AAPCS64)
 *   - BSS zero of real .bss only (linker reserves 32 KiB stack under
 *     __stack_top; that reserve is not wiped — matches linker.ld)
 *   - Clean FP (x29=0) before C; hang with WFE if kmain returns
 *
 * Entry ABI (unchanged):
 *   _start → aarch64_kmain() with no args; no return expected.
 */
.section .text.boot
.global _start
.global aarch64_kmain
.type _start, %function

/* Must match kernel/arch/aarch64/linker.ld early stack reserve. */
.equ EARLY_STACK_BYTES, 0x8000

_start:
    /* Mask Debug/SError/IRQ/FIQ until exceptions + GIC are installed. */
    msr  daifset, #0xf
    isb

    /*
     * Soft EL gate: CurrentEL[3:2] must be 1 (EL1). QEMU -kernel virt
     * enters EL1; higher EL without drop-path would mis-run EL1 code.
     */
    mrs  x0, CurrentEL
    ubfx x0, x0, #2, #2
    cmp  x0, #1
    b.ne 2f

    /* Stack from linker; force 16-byte align for AAPCS64 bl to C. */
    adrp x0, __stack_top
    add  x0, x0, :lo12:__stack_top
    and  x0, x0, #~0xf
    mov  sp, x0

    /*
     * Soft BSS clear: [__bss_start, __stack_top - EARLY_STACK_BYTES).
     * Stack reserve lives inside the .bss VMA; do not zero the stack.
     * Idempotent; safe if loader already zeroed NOBITS.
     */
    adrp x1, __bss_start
    add  x1, x1, :lo12:__bss_start
    sub  x2, x0, #EARLY_STACK_BYTES
    cmp  x1, x2
    b.hs 1f
0:
    str  xzr, [x1], #8
    cmp  x1, x2
    b.lo 0b
1:
    mov  x29, xzr          /* clean frame pointer into freestanding C */
    mov  x0, xzr
    bl   aarch64_kmain

2:
    /* kmain return or wrong EL — soft park */
    wfe
    b    2b

.size _start, .-_start

.section .note.GNU-stack,"",@progbits
