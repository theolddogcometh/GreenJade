/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding ring-3 personality door server (G-PERS product shape).
 * Linked into the kernel image as `gj_protonrt_user_blob` and mapped into a
 * NATIVE process by `user_personality_map` (see README.md).
 *
 * SYSCALL ABI: rax=nr, rdi/rsi/rdx/r10/r8/r9 = args.
 * SYSCALL preserves rbx/r12–r15 (kernel C callee-saved; entry only
 * rebuilds rax/rcx/r11 for sysret). Soft state lives in those regs.
 *
 * GJ_SYS_DEBUG_LOG=0  YIELD=1  IPC_RECV=11  IPC_REPLY=12  PERSONALITY_SERVE=82
 * gj_linux_regs: 8 × u64; i64Ret at offset 56; wire size 64 (frame 128).
 *
 * Hard bring-up banner (DEBUG_LOG, prefix-stable — do not rename):
 *   "protonrt-user: door server up"
 *
 * Soft surface (optional; one-shot; never hard-fails product smoke):
 *   "protonrt-user: soft serve ready"   — first full recv→serve→reply
 *   "protonrt-user: soft recv-miss"     — first soft IPC_RECV fail
 *   "protonrt-user: soft reply-miss"    — first soft IPC_REPLY fail
 *   "protonrt-user: soft serve-miss"    — first serve copy/arg soft fault
 *
 * Soft inventory (Wave 14 exclusive deepen — greppable comments; asm-safe):
 *   greppable: protonrt-user: soft serve ready
 *   greppable: protonrt-user: soft recv-miss
 *   greppable: protonrt-user: soft reply-miss
 *   greppable: protonrt-user: soft serve-miss
 *   greppable: protonrt-user: soft deepen wave=14
 *   greppable: protonrt-user: soft inventory areas=6 wave=14
 * Soft areas (fixed; agent honesty only — not bar3):
 *   1 up-banner   2 fixed-frame   3 wire-zero
 *   4 yield-miss  5 one-shot-soft 6 never-exit-soft
 * Soft flags in r15 (bit inventory, Wave 14):
 *   bit0 SOFT_F_READY  bit1 SOFT_F_RECV
 *   bit2 SOFT_F_REPLY  bit3 SOFT_F_SERVE
 * Soft path (honesty): recv→serve→reply; miss→yield; no EXIT.
 *
 * Soft deepenings (align with kernel cold_personality_server):
 *   - fixed regs frame allocated once (no per-iter stack thrash)
 *   - zero wire-size frame before each IPC_RECV (no stale i64Ret / NR)
 *   - YIELD (+ pause) on soft miss paths (not bare pause alone)
 *   - reply uses PERSONALITY_SERVE rax (same as cold_ipc_service_local ret)
 *   - one-shot soft markers only (no log spam)
 *   - never EXIT / tear-down on soft errors (clients must not hang)
 * Wave 14: soft inventory documented above; no new hard markers or exits.
 *
 * Companion A0 wineserver-shaped smokes live in kernel/main.c (winesrv: …).
 */
.section .text
.global gj_protonrt_user_blob
.global gj_protonrt_user_blob_end

.set GJ_SYS_DEBUG_LOG,         0
.set GJ_SYS_YIELD,             1
.set GJ_SYS_IPC_RECV,         11
.set GJ_SYS_IPC_REPLY,        12
.set GJ_SYS_PERSONALITY_SERVE, 82

/* sizeof(struct gj_linux_regs) == 64; keep 128 for ABI headroom / align. */
.set REGS_FRAME,      128
.set REGS_I64RET_OFF,  56
.set REGS_WIRE,        64

/*
 * Soft flags in r15 (Wave 14 soft inventory bit lamps):
 *   bit0 = soft serve ready logged   (SOFT_F_READY)
 *   bit1 = soft recv-miss logged     (SOFT_F_RECV)
 *   bit2 = soft reply-miss logged    (SOFT_F_REPLY)
 *   bit3 = soft serve-miss logged    (SOFT_F_SERVE)
 * greppable: protonrt-user: soft deepen wave=14
 * greppable: SOFT_F_READY SOFT_F_RECV SOFT_F_REPLY SOFT_F_SERVE
 */
.set SOFT_F_READY,  1
.set SOFT_F_RECV,   2
.set SOFT_F_REPLY,  4
.set SOFT_F_SERVE,  8
/* Wave 14 soft inventory area count (comment-only stamp; not a runtime). */
.set SOFT_INV_WAVE,  14
.set SOFT_INV_AREAS,  6

gj_protonrt_user_blob:
    xor  %r15, %r15                 /* soft flag word */
    sub  $REGS_FRAME, %rsp
    mov  %rsp, %r14                 /* r14 = fixed gj_linux_regs frame */

    mov  $GJ_SYS_DEBUG_LOG, %rax
    lea  msg_up(%rip), %rdi
    mov  $msg_up_len, %rsi
    syscall

.Lloop:
    /* Soft hygiene: zero wire-size regs so stale i64Ret cannot leak. */
    cld
    mov  %r14, %rdi
    xor  %eax, %eax
    mov  $(REGS_WIRE / 8), %ecx
    rep  stosq

    mov  %r14, %rdi
    mov  $GJ_SYS_IPC_RECV, %rax
    syscall
    test %rax, %rax
    jnz  .Lrecv_soft

    /* Cold service (vfs_ram / net_lo / stubs) via PERSONALITY_SERVE */
    mov  %r14, %rdi
    mov  $GJ_SYS_PERSONALITY_SERVE, %rax
    syscall
    mov  %rax, %rbx                 /* rbx = serve return → door reply */

    /*
     * Soft serve-miss: copy/arg fault leaves frame i64Ret at 0 (we zeroed)
     * while rax carries a negative GJ errno. Linux cold policy returns
     * (incl. -ENOSYS) copy out into the frame, so they skip this marker.
     * Always reply with rax so the client cannot hang (G-COLD-3 spirit).
     */
    mov  REGS_I64RET_OFF(%r14), %rcx
    test %rcx, %rcx
    jnz  .Lreply
    test %rbx, %rbx
    jns  .Lreply
    test $SOFT_F_SERVE, %r15
    jnz  .Lreply
    or   $SOFT_F_SERVE, %r15
    mov  $GJ_SYS_DEBUG_LOG, %rax
    lea  msg_serve_miss(%rip), %rdi
    mov  $msg_serve_miss_len, %rsi
    syscall

.Lreply:
    /* Match cold_personality_server: door_reply(serve_ret). */
    mov  %rbx, %rdi
    mov  $GJ_SYS_IPC_REPLY, %rax
    syscall
    test %rax, %rax
    jnz  .Lreply_soft

    /* Soft ready: first full flight completed. */
    test $SOFT_F_READY, %r15
    jnz  .Lloop
    or   $SOFT_F_READY, %r15
    mov  $GJ_SYS_DEBUG_LOG, %rax
    lea  msg_ready(%rip), %rdi
    mov  $msg_ready_len, %rsi
    syscall
    jmp  .Lloop

.Lrecv_soft:
    /* Soft: one-shot marker, then YIELD (kernel cold server uses yield). */
    test $SOFT_F_RECV, %r15
    jnz  .Lyield
    or   $SOFT_F_RECV, %r15
    mov  $GJ_SYS_DEBUG_LOG, %rax
    lea  msg_recv_miss(%rip), %rdi
    mov  $msg_recv_miss_len, %rsi
    syscall
    jmp  .Lyield

.Lreply_soft:
    test $SOFT_F_REPLY, %r15
    jnz  .Lyield
    or   $SOFT_F_REPLY, %r15
    mov  $GJ_SYS_DEBUG_LOG, %rax
    lea  msg_reply_miss(%rip), %rdi
    mov  $msg_reply_miss_len, %rsi
    syscall
    /* fall through */

.Lyield:
    pause
    mov  $GJ_SYS_YIELD, %rax
    syscall
    jmp  .Lloop

/* ---- markers (ASCII, prefix-stable) ---- */

msg_up:
    .ascii "protonrt-user: door server up\n"
.set msg_up_len, . - msg_up

msg_ready:
    .ascii "protonrt-user: soft serve ready\n"
.set msg_ready_len, . - msg_ready

msg_recv_miss:
    .ascii "protonrt-user: soft recv-miss\n"
.set msg_recv_miss_len, . - msg_recv_miss

msg_reply_miss:
    .ascii "protonrt-user: soft reply-miss\n"
.set msg_reply_miss_len, . - msg_reply_miss

msg_serve_miss:
    .ascii "protonrt-user: soft serve-miss\n"
.set msg_serve_miss_len, . - msg_serve_miss

gj_protonrt_user_blob_end:

.section .note.GNU-stack,"",@progbits
