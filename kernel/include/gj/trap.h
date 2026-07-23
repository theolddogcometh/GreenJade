/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception / IRQ trap frame + soft #PF COW path observability (G-IDT-*).
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope
 * -------------------------------------------------------------------------
 * C-visible stack frame built by ISR stubs + common entry, and the soft
 * trap_dispatch path that classifies exceptions, PE32 smoke enters, and
 * user #PF copy-on-write candidates. Soft deepen: trap path counters +
 * greppable #PF COW decision logs. Soft never hard-gates product paths.
 *
 * Frame layout contract
 * ---------------------
 * Hardware pushes (with error code when applicable) then stubs push the
 * general registers in reverse order so C sees struct gj_trap_frame with
 * R15 at the lowest address and SS at the highest. Do not reorder fields
 * without matching isr / common asm.
 *
 *   GPRs (R15..RAX) → u64Vector → u64Error → RIP CS RFLAGS RSP SS
 *
 * #PF COW soft pipeline (user present+write)
 * ------------------------------------------
 *   u64PfUser → u64PfCowCand → (u64PfCowCr3Sw) → u64PfCowOk | u64PfCowMiss
 * Non-candidate user #PF: u64PfCowSkip, then except/kill fallthrough.
 * Exception port delivery stays in except.c; trap only counts outcomes.
 *
 * Classification lamps
 * --------------------
 *   u64User / u64Kernel   by CS RPL
 *   u64VecLt32 / Ge32     exception vs IRQ/soft vector
 *   u64Pe32Bp / Int80     PE32 CS32 hardware-enter smoke
 *   u64Except* / u64Kill  fault-port vs kill fallthrough
 *
 * greppable: trap: soft stats
 * greppable: trap: #PF soft
 * greppable: trap_dispatch trap_stats_soft
 * greppable: G-IDT COW PE32
 */
#pragma once

#include <gj/types.h>

/**
 * Stack frame built by isr stubs + common (reverse push order).
 * Layout is an asm contract — keep field order stable.
 */
struct gj_trap_frame {
    u64 u64R15;
    u64 u64R14;
    u64 u64R13;
    u64 u64R12;
    u64 u64R11;
    u64 u64R10;
    u64 u64R9;
    u64 u64R8;
    u64 u64Rbp;
    u64 u64Rdi;
    u64 u64Rsi;
    u64 u64Rdx;
    u64 u64Rcx;
    u64 u64Rbx;
    u64 u64Rax;
    u64 u64Vector;
    u64 u64Error;
    u64 u64Rip;
    u64 u64Cs;
    u64 u64Rflags;
    u64 u64Rsp;
    u64 u64Ss;
};

/*
 * Soft trap-dispatch counters (wrap OK; diagnostics only — never hard-gate).
 * Grep: trap: soft stats
 * Grep: trap: #PF soft
 *
 * #PF COW path (user present+write):
 *   u64PfUser → u64PfCowCand → (u64PfCowCr3Sw) → u64PfCowOk | u64PfCowMiss
 * Non-candidate user #PF: u64PfCowSkip, then except/kill fallthrough.
 * Exception port delivery stays in except.c; trap only counts outcomes.
 */
struct gj_trap_stats {
    u64 u64Total;         /* trap_dispatch entries (non-null frame) */
    u64 u64NullFrame;     /* null frame (halt) */
    u64 u64User;          /* ring-3 traps */
    u64 u64Kernel;        /* ring-0 traps (halt path) */
    u64 u64VecLt32;       /* CPU exception vectors 0..31 */
    u64 u64VecGe32;       /* IRQ / soft vectors >= 32 */
    u64 u64Pe32Bp;        /* PE32 CS32 #BP hw-enter */
    u64 u64Pe32Int80;     /* PE32 CS32 int 0x80 */
    u64 u64PfUser;        /* user #PF entered soft COW path */
    u64 u64PfCowCand;     /* err present+write (COW candidate) */
    u64 u64PfCowCr3Sw;    /* loaded process CR3 before break try */
    u64 u64PfCowOk;       /* vmm_cow_break_page OK → resume */
    u64 u64PfCowMiss;     /* candidate but break failed (not COW/nomem) */
    u64 u64PfCowSkip;     /* user #PF not present+write */
    u64 u64ExceptDeliver; /* except_port_deliver posted (thr blocks) */
    u64 u64ExceptMiss;    /* no live port / no proc → kill fallthrough */
    u64 u64Kill;          /* user kill path taken */
};

/**
 * Common C entry from ISR stubs.
 * Classifies frame (user/kernel, vector class, PE32, #PF COW) and either
 * resumes, posts an exception port, kills the user thread, or halts on
 * kernel faults / null frame. Soft-bumps gj_trap_stats on every path.
 */
void trap_dispatch(struct gj_trap_frame *pFrame);

/** Snapshot soft counters into *pOut (NULL → no-op). */
void trap_stats_get(struct gj_trap_stats *pOut);

/** Clear soft trap counters. */
void trap_stats_reset(void);

/**
 * Greppable soft stats line: "trap: soft stats ..."
 * Returns u64Total (handy for smoke without parsing).
 * greppable: trap: soft stats
 */
u64 trap_stats_soft(void);
