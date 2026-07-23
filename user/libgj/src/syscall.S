/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * long gj_syscall6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
 * SysV AMD64: rdi,rsi,rdx,rcx,r8,r9, stack+8
 * Linux/GJ SYSCALL: rax=nr, rdi,rsi,rdx,r10,r8,r9  (rcx clobbered by SYSCALL)
 */
.section .text
.global gj_syscall6
gj_syscall6:
    mov %rdi, %rax          /* nr */
    mov %rsi, %rdi          /* a0 */
    mov %rdx, %rsi          /* a1 */
    mov %rcx, %rdx          /* a2 */
    mov %r8,  %r10          /* a3 → r10 (not rcx) */
    mov %r9,  %r8           /* a4 */
    mov 8(%rsp), %r9        /* a5 from stack */
    syscall
    ret

.section .note.GNU-stack,"",@progbits
