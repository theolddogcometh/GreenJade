/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Position-independent ring-3 trampoline (copied to user VA).
 * Exercises SYSCALL getpid + write + exit.
 */
.section .rodata
.global gj_user_ring3_blob
.global gj_user_ring3_blob_end
.global gj_user_ring3_msg_off

/* Offsets relative to blob start when relocated to entry VA. */
gj_user_ring3_blob:
    /* getpid */
    mov $39, %rax
    syscall
    /* write(1, msg, 9) — lea msg relative */
    mov $1, %rax
    mov $1, %rdi
    lea gj_user_ring3_msg(%rip), %rsi
    mov $9, %rdx
    syscall
    /* exit(0) */
    mov $60, %rax
    xor %rdi, %rdi
    syscall
    /* hang if exit returns */
1:  jmp 1b
.align 8
gj_user_ring3_msg:
    .ascii "ring3 ok\n"
gj_user_ring3_blob_end:

/* byte offset of message from blob start (for debug) */
.set gj_user_ring3_msg_off, gj_user_ring3_msg - gj_user_ring3_blob
