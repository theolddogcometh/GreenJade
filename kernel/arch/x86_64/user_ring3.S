/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Position-independent ring-3 trampoline (copied to user VA).
 *
 * Soft deepen (ABI-stable blob contract for user_task_map_ring3):
 *   - Machine code is position-independent (RIP-relative lea for msg).
 *   - Copied to GJ_USER_CODE_VA; stack at GJ_USER_STACK_TOP (see user_task).
 *   - Exercises Linux x86_64 SYSCALL path (personality LINUX):
 *       getpid  nr=39
 *       write   nr=1   (fd=1, buf=msg, len=9) → "ring3 ok\n"
 *       exit    nr=60  (status=0)
 *   - Symbols consumed by C:
 *       gj_user_ring3_blob / _end  — [start, end) byte range for map/copy
 *       gj_user_ring3_msg_off      — byte offset of message from blob start
 *   - Soft recheck compares live PTEs + payload against this blob; do not
 *     change instruction bytes casually without expecting map-soft reverify.
 *
 * Grep: user: ring3 map soft, gj_user_ring3_blob
 */
.section .rodata
.global gj_user_ring3_blob
.global gj_user_ring3_blob_end
.global gj_user_ring3_msg_off

/* Offsets relative to blob start when relocated to entry VA. */
.type gj_user_ring3_blob, @object
gj_user_ring3_blob:
    /* getpid — smoke that LSTAR/sysret + nr route work */
    mov $39, %rax
    syscall
    /* write(1, msg, 9) — lea msg RIP-relative (PIC after copy) */
    mov $1, %rax
    mov $1, %rdi
    lea gj_user_ring3_msg(%rip), %rsi
    mov $9, %rdx
    syscall
    /* exit(0) — thr terminates via Linux exit path */
    mov $60, %rax
    xor %rdi, %rdi
    syscall
    /* hang if exit returns (should not) */
1:  jmp 1b
.align 8
gj_user_ring3_msg:
    .ascii "ring3 ok\n"
gj_user_ring3_blob_end:

/* byte offset of message from blob start (for debug / soft tools) */
.set gj_user_ring3_msg_off, gj_user_ring3_msg - gj_user_ring3_blob
/* Ensure absolute offset is a visible linker symbol (C may reference). */
.globl gj_user_ring3_msg_off

.section .note.GNU-stack,"",@progbits
