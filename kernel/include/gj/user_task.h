/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Ring-3 trampoline + freestanding personality server map (G-PERS).
 * Pure C freestanding; dual MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Install fixed user VA windows for:
 *   1) Linux-shaped ring-3 smoke trampoline (sysret enter)
 *   2) Freestanding protonrt-user door server (native IPC_* personality)
 *
 * Both paths fail closed on soft map verify (PTE P|U, code RX / stack RW,
 * payload soft match, VA window inside user band, non-overlapping layout).
 *
 * Product VA layout
 * -----------------
 * High enough to clear kernel identity BSS / embeds; stays inside the
 * canonical user band [GJ_USER_VA_BASE, GJ_USER_VA_END):
 *
 *   GJ_USER_CODE_VA / STACK_TOP  — ring-3 Linux smoke
 *   GJ_PERS_CODE_VA / STACK_TOP  — native personality server
 *
 * Soft product surface
 * --------------------
 *   greppable: "user: soft stats"
 *   greppable: "user: ring3 map soft"
 *   greppable: "user: personality map soft"
 *   Post-map PTE soft verify + payload soft match
 *   Cumulative map ok/fail + soft PASS/soft_bad + enter counters
 *
 * Enter contract
 * --------------
 * user_task_enter_ring3 does not return on success (sysret). Soft re-verify
 * before enter; greppable soft skip when not mapped / SYSCALL not ready.
 *
 * Related: gj/process.h, gj/thread.h (create_user), gj/syscall.h,
 *          gj/cold_ipc.h (personality server), gj/elf_load.h (full ELF path).
 * docs/PROTON_PERSONALITY.md · docs/LINUX_ABI_HYBRID.md
 */
#pragma once

#include <gj/error.h>
#include <gj/process.h>
#include <gj/types.h>

/*
 * Product VA layout (high enough to clear kernel identity BSS / embeds).
 * Windows stay inside the canonical user band [GJ_USER_VA_BASE, GJ_USER_VA_END).
 * Stack grows down from *_STACK_TOP; code is RX soft-verified pages.
 */
#define GJ_USER_CODE_VA       0x0000000001000000ull
#define GJ_USER_STACK_TOP     0x0000000001100000ull
#define GJ_USER_STACK_PAGES   4u
#define GJ_USER_CODE_PAGES    4u /* soft max multi-page trampoline / door blob */
#define GJ_PERS_CODE_VA       0x0000000001200000ull
#define GJ_PERS_STACK_TOP     0x0000000001300000ull
#define GJ_PERS_STACK_PAGES   GJ_USER_STACK_PAGES
#define GJ_PERS_CODE_PAGES    GJ_USER_CODE_PAGES

/**
 * Soft map / enter counters (observability; not a hard ABI).
 * Grep: user: soft stats
 *
 * u32Ring3Mapped / u32PersMapped are live 0/1 mirrors (not cumulative).
 * Soft PASS means post-map PTE + payload verify succeeded.
 */
struct gj_user_task_stats {
    u32 u32Ring3Ok;       /* successful user_task_map_ring3 */
    u32 u32Ring3Fail;     /* failed user_task_map_ring3 */
    u32 u32Ring3Soft;     /* post-map soft verify PASS */
    u32 u32Ring3SoftBad;  /* post-map soft verify FAIL */
    u32 u32PersOk;        /* successful user_personality_map */
    u32 u32PersFail;      /* failed user_personality_map */
    u32 u32PersSoft;      /* personality post-map soft PASS */
    u32 u32PersSoftBad;   /* personality post-map soft FAIL */
    u32 u32EnterOk;       /* user_task_enter_ring3 reached cpu_enter_user */
    u32 u32EnterSkip;     /* enter refused (not mapped / SYSCALL not ready) */
    u32 u32CodePages;     /* cumulative code pages installed (both windows) */
    u32 u32StackPages;    /* cumulative stack pages installed */
    u32 u32Ring3Mapped;   /* 0/1 live ring3 soft-mapped flag */
    u32 u32PersMapped;    /* 0/1 live personality soft-mapped flag */
};

/**
 * Map Linux-shaped ring-3 smoke trampoline into pProc AS (soft post-map verify).
 * Code @ GJ_USER_CODE_VA, stack top @ GJ_USER_STACK_TOP (grows down).
 * Sets LINUX personality (1), JIT off. Fail closed on soft map bad.
 * Grep: user: ring3 map soft PASS|FAIL
 * Returns 0 on success, -1 on failure.
 */
int user_task_map_ring3(struct gj_process *pProc);

/**
 * Enter ring3 via sysret (does not return on success).
 * Soft re-verify of mapped windows before enter; greppable soft skip on refuse.
 * Requires prior successful user_task_map_ring3 and ready LSTAR/sysret path.
 */
void user_task_enter_ring3(void);

/**
 * Map freestanding protonrt-user door server into process AS (G-PERS).
 * Entry @ GJ_PERS_CODE_VA, stack top @ GJ_PERS_STACK_TOP.
 * Sets NATIVE personality (0) for IPC_* syscalls. Fail closed on soft map bad.
 * Grep: user: personality map soft PASS|FAIL
 * Returns 0 on success, -1 on failure.
 */
int user_personality_map(struct gj_process *pProc);

/** VA of mapped personality entry / stack top (0 if not mapped). */
u64 user_personality_entry(void);
u64 user_personality_stack(void);

/** Non-zero if ring3 / personality soft map currently live. */
int user_task_ring3_mapped(void);
int user_personality_mapped(void);

/**
 * Snapshot soft map/enter stats into *pOut.
 * pOut NULL → GJ_ERR_INVAL.
 */
gj_status_t user_task_stats(struct gj_user_task_stats *pOut);

/**
 * Greppable soft stats line: "user: soft stats ..."
 * Returns ring3 soft PASS count (handy for smoke without parsing).
 */
u32 user_task_stats_soft(void);

/**
 * Soft re-verify of a previously mapped ring3 window under the active CR3.
 * Does not install pages. Returns 0 on soft PASS, -1 on soft FAIL.
 * Grep: user: ring3 map soft recheck
 */
int user_task_ring3_map_soft(void);

/**
 * Soft re-verify of a previously mapped personality window under active CR3.
 * Does not install pages. Returns 0 on soft PASS, -1 on soft FAIL.
 * Grep: user: personality map soft recheck
 */
int user_personality_map_soft(void);
