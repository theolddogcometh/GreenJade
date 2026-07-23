/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * void switch_context(u64 *pOldRsp, u64 u64NewRsp);
 * Saves callee-saved regs + rip on old stack; switches to newRsp; returns to
 * the rip popped from the new stack.
 */
.section .text
.global switch_context
switch_context:
    /* rdi = &old->u64Rsp, rsi = new->u64Rsp */
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15
    mov %rsp, (%rdi)
    mov %rsi, %rsp
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret

.section .note.GNU-stack,"",@progbits
