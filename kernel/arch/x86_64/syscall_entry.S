/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL with swapgs + per-CPU kernel stack (struct gj_cpu at GS).
 *
 * Soft deepen (ABI-stable LSTAR edge - do not reorder without C/header sync):
 *
 *   Offsets must match kernel/include/gj/cpu.h leading fields:
 *     +0  u64KernelRsp     gs:CPU_KERNEL_RSP
 *     +8  u64UserRsp       gs:CPU_USER_RSP
 *     +16 u64UserRip       gs:CPU_USER_RIP
 *     +24 u64UserRflags    gs:CPU_USER_RFLAGS
 *     +32 pCurrent         (C only)
 *     +40 u64Cr3           (C only)
 *     +48 u64UserRbx       gs:CPU_USER_RBX  (SYSCALL-entry; before C)
 *     +56 u64UserRbp       gs:CPU_USER_RBP
 *     +64 u64UserR12       gs:CPU_USER_R12
 *     +72 u64UserR13       gs:CPU_USER_R13
 *     +80 u64UserR14       gs:CPU_USER_R14
 *     +88 u64UserR15       gs:CPU_USER_R15
 *
 *   Hardware SYSCALL state on entry:
 *     RCX = user RIP, R11 = user RFLAGS, RSP = user stack
 *     RAX = nr; RDI/RSI/RDX/R10/R8/R9 = arg0..5 (Linux x86_64)
 *     GS  = user GS; KERNEL_GS_BASE = per-CPU struct (after swapgs -> GS)
 *
 *   Stack frame built for gj_syscall_entry_asm_bridge / struct gj_syscall_regs
 *   (low -> high address; matches syscall.h field order):
 *     +0x00  u64Nr
 *     +0x08  u64Arg0   (rdi)
 *     +0x10  u64Arg1   (rsi)
 *     +0x18  u64Arg2   (rdx)
 *     +0x20  u64Arg3   (r10)
 *     +0x28  u64Arg4   (r8)
 *     +0x30  u64Arg5   (r9)
 *     +0x38  i64Ret    (placeholder; C writes return value here)
 *
 *   Exit: load i64Ret -> RAX, reload RDI/RSI/RDX/R10/R8/R9 from the frame
 *   (Linux x86_64 SYSCALL preserves those; C clobbers them), restore
 *   RCX/R11/RSP from GS shadows, swapgs, sysretq. Bridge may yield;
 *   schedule() save/restores GS USER_* so mid-syscall block returns to
 *   the correct user context (see thread.h).
 *
 * Soft residual (C3 arch LSTAR entry; Soft!=product; Dual DoD A/B OPEN):
 *   Product path only: swapgs, per-CPU KERNEL_RSP, build gj_syscall_regs,
 *   call bridge, load i64Ret, restore USER_* shadows, swapgs, sysretq.
 *   Soft counters / residual lamps live in entry_bridge.c + dispatch.c
 *   (SYSCALL_ENTRY_SOFT_STATS). Soft inventory never hard-gates sysretq.
 *   Soft!=product; dual MIT OR Apache-2.0; G-AC-1 (no .ko product AC).
 *   H1: KERNEL_RSP per-CPU only - never thr IRQ stack; never net_eth_poll.
 *   H2: no kprintf on hot LSTAR land (storm=0; residual is C once-lamp only).
 *   H3: exit restores USER_RIP/RFLAGS/RSP from GS; schedule must keep
 *       shadows coherent across yield (thread_install_kstack).
 *   Dual DoD A/B remain OPEN - agent residual never closes product DoD.
 *   stamp-free residual (no version stamp; no Softneq; ASCII Soft!=product).
 *
 * greppable: Soft!=product
 * greppable: syscall_entry: soft residual
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 * Grep: gj_syscall_entry, SYSCALL_ENTRY_SOFT_STATS, Soft!=product
 */
/* struct gj_cpu offsets — keep in lockstep with gj/cpu.h */
.set CPU_KERNEL_RSP, 0
.set CPU_USER_RSP,   8
.set CPU_USER_RIP,   16
.set CPU_USER_RFLAGS,24
.set CPU_USER_RBX,   48
.set CPU_USER_RBP,   56
.set CPU_USER_R12,   64
.set CPU_USER_R13,   72
.set CPU_USER_R14,   80
.set CPU_USER_R15,   88

/* struct gj_syscall_regs size and field offsets (8 × u64). */
.set SYSCALL_REGS_SIZE, 64
.set SYSCALL_REGS_ARG0, 8
.set SYSCALL_REGS_ARG1, 16
.set SYSCALL_REGS_ARG2, 24
.set SYSCALL_REGS_ARG3, 32
.set SYSCALL_REGS_ARG4, 40
.set SYSCALL_REGS_ARG5, 48
.set SYSCALL_REGS_RET,  56

.section .text
.global gj_syscall_entry
.type gj_syscall_entry, @function
.extern gj_syscall_entry_asm_bridge

gj_syscall_entry:
    swapgs
    mov %rsp, %gs:CPU_USER_RSP
    mov %gs:CPU_KERNEL_RSP, %rsp
    mov %rcx, %gs:CPU_USER_RIP
    mov %r11, %gs:CPU_USER_RFLAGS
    /* User callee-saves before C (fork child; C rbp is a kernel frame). */
    mov %rbx, %gs:CPU_USER_RBX
    mov %rbp, %gs:CPU_USER_RBP
    mov %r12, %gs:CPU_USER_R12
    mov %r13, %gs:CPU_USER_R13
    mov %r14, %gs:CPU_USER_R14
    mov %r15, %gs:CPU_USER_R15

    /* Build gj_syscall_regs on the per-CPU syscall stack (top-down pushes). */
    push %rax                 /* i64Ret placeholder (filled by dispatch) */
    push %r9                  /* arg5 */
    push %r8                  /* arg4 */
    push %r10                 /* arg3 */
    push %rdx                 /* arg2 */
    push %rsi                 /* arg1 */
    push %rdi                 /* arg0 */
    push %rax                 /* nr */
    mov %rsp, %rdi            /* &struct gj_syscall_regs */
    cld                       /* SysV: DF clear before C */
    call gj_syscall_entry_asm_bridge
    mov SYSCALL_REGS_RET(%rsp), %rax
    /* Linux x86_64 SYSCALL preserves arg regs; C clobbers them. */
    mov SYSCALL_REGS_ARG0(%rsp), %rdi
    mov SYSCALL_REGS_ARG1(%rsp), %rsi
    mov SYSCALL_REGS_ARG2(%rsp), %rdx
    mov SYSCALL_REGS_ARG3(%rsp), %r10
    mov SYSCALL_REGS_ARG4(%rsp), %r8
    mov SYSCALL_REGS_ARG5(%rsp), %r9
    add $SYSCALL_REGS_SIZE, %rsp

    mov %gs:CPU_USER_RIP, %rcx
    mov %gs:CPU_USER_RFLAGS, %r11
    mov %gs:CPU_USER_RSP, %rsp
    swapgs
    sysretq

.size gj_syscall_entry, . - gj_syscall_entry

.section .note.GNU-stack,"",@progbits
