/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative context switch (kernel threads).
 *
 *   void switch_context(u64 *pOldRsp, u64 u64NewRsp);
 *     rdi = &old->u64Rsp
 *     rsi = new->u64Rsp   (absolute stack pointer value, not a pointer-to)
 *
 * Soft deepen (ABI-stable frame contract — thr_build_initial_rsp must match):
 *
 *   Saved frame layout at *pOldRsp after switch (low → high address):
 *     +0x00  r15
 *     +0x08  r14
 *     +0x10  r13
 *     +0x18  r12
 *     +0x20  rbx
 *     +0x28  rbp
 *     +0x30  rip   (return address; ret pops this)
 *
 *   thr_build_initial_rsp (thread.c) pushes in reverse so the first resume
 *   pops r15..rbp then rets into thread_trampoline. Do not reorder pushes
 *   without updating thr_build_initial_rsp and any hand-built frames.
 *
 *   Only SysV callee-saved GPRs are preserved (rbx, rbp, r12–r15). Caller-
 *   saved regs are live only across C calls, not across schedule().
 *   Does not touch CR3, GS, FPU/SSE, or per-CPU SYSCALL user shadows —
 *   those are the scheduler / thread_install_kstack responsibility.
 *
 * Grep: switch_context, thr_build_initial_rsp
 */
.section .text
.global switch_context
.type switch_context, @function
switch_context:
    /* rdi = &old->u64Rsp, rsi = new->u64Rsp */
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15
    mov %rsp, (%rdi)       /* *pOldRsp = current SP (points at r15) */
    mov %rsi, %rsp         /* switch to new stack */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret                    /* → saved rip (trampoline or prior call site) */

.size switch_context, . - switch_context

.section .note.GNU-stack,"",@progbits
