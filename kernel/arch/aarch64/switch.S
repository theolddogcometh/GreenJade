/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative context switch for shared sched_coop (AArch64).
 * void gj_coop_switch(u64 *pOldSp, u64 u64NewSp);
 *   x0 = pOldSp, x1 = u64NewSp
 * Saves callee-saved (x19-x30) on old stack, stores SP to *pOldSp,
 * loads new SP, restores callee-saved, returns (via x30 on new stack).
 */
.section .text
.global gj_coop_switch
.align 2

gj_coop_switch:
    /* Save frame */
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    /* Callee-saved */
    stp x19, x20, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x27, x28, [sp, #-16]!
    /* *pOldSp = sp */
    mov x2, sp
    str x2, [x0]
    /* sp = u64NewSp */
    mov sp, x1
    /* Restore callee-saved from new stack */
    ldp x27, x28, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16
    ldp x29, x30, [sp], #16
    ret

.section .note.GNU-stack,"",@progbits
