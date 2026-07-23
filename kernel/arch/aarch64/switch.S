/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative context switch for shared sched_coop (AArch64).
 *
 * void gj_coop_switch(u64 *pOldSp, u64 u64NewSp);
 *   x0 = pOldSp, x1 = u64NewSp   (AAPCS64)
 *
 * Soft deepen keeps the frame ABI fixed so sched_coop.c create path matches:
 *
 *   After save, *pOldSp points at a 12-slot (96-byte) block, low→high:
 *     [0]  x27    [1]  x28
 *     [2]  x25    [3]  x26
 *     [4]  x23    [5]  x24
 *     [6]  x21    [7]  x22
 *     [8]  x19    [9]  x20
 *     [10] x29    [11] x30   (LR / trampoline entry for new thr)
 *
 * Restore order is the inverse of the save pushes. New threads are minted
 * with x30 = coop_trampoline; first restore returns into that entry.
 *
 * Soft notes (not changed here — product later):
 *   - FP/SIMD d8–d15 not saved (coop soft path is integer-only today)
 *   - Caller must not pass unaligned u64NewSp (create path enforces 16B)
 *   - x0/x1/x2 are scratch after *pOldSp store; void return via ret
 */
.section .text
.global gj_coop_switch
.type gj_coop_switch, %function
.align 2

gj_coop_switch:
    /* High half of frame: FP/LR first so restore ends with ret via x30. */
    stp  x29, x30, [sp, #-16]!
    mov  x29, sp

    /* Callee-saved pairs (AAPCS64 x19–x28); order must match sched_coop. */
    stp  x19, x20, [sp, #-16]!
    stp  x21, x22, [sp, #-16]!
    stp  x23, x24, [sp, #-16]!
    stp  x25, x26, [sp, #-16]!
    stp  x27, x28, [sp, #-16]!

    /* *pOldSp = sp  (points at x27/x28) */
    mov  x2, sp
    str  x2, [x0]

    /* sp = u64NewSp */
    mov  sp, x1

    /* Restore callee-saved from new stack (low → high). */
    ldp  x27, x28, [sp], #16
    ldp  x25, x26, [sp], #16
    ldp  x23, x24, [sp], #16
    ldp  x21, x22, [sp], #16
    ldp  x19, x20, [sp], #16
    ldp  x29, x30, [sp], #16
    ret

.size gj_coop_switch, .-gj_coop_switch

.section .note.GNU-stack,"",@progbits
