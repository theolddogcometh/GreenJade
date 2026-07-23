/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL with swapgs + per-CPU kernel stack (struct gj_cpu at GS).
 * Offsets must match kernel/include/gj/cpu.h
 */
/* struct gj_cpu offsets */
.set CPU_KERNEL_RSP, 0
.set CPU_USER_RSP,   8
.set CPU_USER_RIP,   16
.set CPU_USER_RFLAGS,24

.section .text
.global gj_syscall_entry
.extern gj_syscall_entry_asm_bridge

gj_syscall_entry:
    swapgs
    mov %rsp, %gs:CPU_USER_RSP
    mov %gs:CPU_KERNEL_RSP, %rsp
    mov %rcx, %gs:CPU_USER_RIP
    mov %r11, %gs:CPU_USER_RFLAGS

    push %rax                 /* ret placeholder */
    push %r9
    push %r8
    push %r10
    push %rdx
    push %rsi
    push %rdi
    push %rax                 /* nr */
    mov %rsp, %rdi
    call gj_syscall_entry_asm_bridge
    mov 56(%rsp), %rax
    add $64, %rsp

    mov %gs:CPU_USER_RIP, %rcx
    mov %gs:CPU_USER_RFLAGS, %r11
    mov %gs:CPU_USER_RSP, %rsp
    swapgs
    sysretq

.section .note.GNU-stack,"",@progbits
