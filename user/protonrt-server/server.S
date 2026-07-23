/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding ring-3 personality door server (G-PERS product shape).
 * Linked into the kernel image as `gj_protonrt_user_blob` and mapped into a
 * NATIVE process by `user_personality_map` (see README.md).
 *
 * SYSCALL ABI: rax=nr, rdi/rsi/rdx/r10/r8/r9 = args.
 *
 * GJ_SYS_DEBUG_LOG=0  IPC_RECV=11  IPC_REPLY=12  PERSONALITY_SERVE=82
 * gj_linux_regs: i64Ret at offset 56.
 *
 * Boot marker (DEBUG_LOG): "protonrt-user: door server up"
 * Companion A0 wineserver-shaped smokes live in kernel/main.c (winesrv: …).
 */
.section .text
.global gj_protonrt_user_blob
.global gj_protonrt_user_blob_end

.set GJ_SYS_DEBUG_LOG, 0
.set GJ_SYS_IPC_RECV,  11
.set GJ_SYS_IPC_REPLY, 12
.set GJ_SYS_PERSONALITY_SERVE, 82
.set REGS_I64RET_OFF, 56

gj_protonrt_user_blob:
    mov $GJ_SYS_DEBUG_LOG, %rax
    lea msg_start(%rip), %rdi
    mov $msg_len, %rsi
    syscall

.Lloop:
    sub $128, %rsp                 /* buffer for gj_linux_regs */
    mov %rsp, %rdi
    mov $GJ_SYS_IPC_RECV, %rax
    syscall
    test %rax, %rax
    jnz .Lfail

    /* Cold service (vfs_ram / net_lo / stubs) via PERSONALITY_SERVE */
    mov %rsp, %rdi
    mov $GJ_SYS_PERSONALITY_SERVE, %rax
    syscall
    /* rax = i64Ret; also stored at buf+56 */
    mov REGS_I64RET_OFF(%rsp), %rdi
    mov $GJ_SYS_IPC_REPLY, %rax
    syscall
    add $128, %rsp
    jmp .Lloop

.Lfail:
    add $128, %rsp
    pause
    jmp .Lloop

msg_start:
    .ascii "protonrt-user: door server up\n"
.set msg_len, . - msg_start

gj_protonrt_user_blob_end:

.section .note.GNU-stack,"",@progbits
