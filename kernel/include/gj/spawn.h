/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * posix_spawn-like process create + PROCESS task cap (G-PROC-*).
 * Product lifecycle: spawn → kill (process_death) → wait (reap + recycle slot).
 *
 * Soft product surface (grep: spawn: soft | spawn: mint soft):
 *   PROCESS cap mint verify (type/rights/obj/gen) after parent install
 *   Cumulative + live + fail/kill/wait/mint counters
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

#define GJ_SPAWN_CNODE_SLOTS 64ull
#define GJ_SPAWN_MAX         8u

/* Default PROCESS task rights (G-PROC-2); JIT is OR'd when CapJit is set. */
#define GJ_SPAWN_PROCESS_RIGHTS_BASE                                       \
    ((u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY | GJ_RIGHT_DESTROY |          \
           GJ_RIGHT_WAIT | GJ_RIGHT_VM | GJ_RIGHT_SPAWN))

/* Core task rights soft-required after mint (kill/reap/vm surface). */
#define GJ_SPAWN_PROCESS_RIGHTS_CORE                                       \
    ((u16)(GJ_RIGHT_DESTROY | GJ_RIGHT_WAIT | GJ_RIGHT_VM))

struct gj_spawn_args {
    void              (*pfnEntry)(void *pArg);
    void               *pArg;
    u32                 u32Jit;         /* CapJit for child */
    u32                 u32Personality; /* 0 native, 1 linux */
};

/**
 * Soft spawn / PROCESS-mint counters (observability; not a hard ABI).
 * Grep: spawn: soft stats
 */
struct gj_spawn_stats {
    u32 u32Ok;         /* cumulative successful process_spawn */
    u32 u32Fail;       /* cumulative failed process_spawn */
    u32 u32Live;       /* occupied fixed spawn slots */
    u32 u32Kill;       /* process_kill that reached death (or idempotent ok) */
    u32 u32Wait;       /* process_wait reaps */
    u32 u32MintOk;     /* PROCESS cap mint install ok */
    u32 u32MintFail;   /* PROCESS cap mint install fail */
    u32 u32MintSoft;   /* soft post-mint verify PASS */
    u32 u32MintSoftBad;/* soft post-mint verify FAIL */
};

/**
 * Create child process: CNode, root meta, private AS, first thread.
 * Mints GJ_CAP_PROCESS into parent CNode (G-PROC-2) with soft post-mint
 * type/rights/obj/gen verify. Wait-registers child so process_death
 * reclaims AS/CNode (G-PROC-5).
 * On any failure after partial install: reverse tear down (no orphan AS/cap).
 * Returns GJ_OK and fills *ppOutChild / *pOutCap (either may be NULL).
 */
gj_status_t process_spawn(struct gj_process *pParent,
                          const struct gj_spawn_args *pArgs,
                          struct gj_process **ppOutChild,
                          struct gj_cap_ref *pOutCap);

/** Look up child by PROCESS cap in parent CNode. */
struct gj_process *process_from_cap(struct gj_process *pParent,
                                    const struct gj_cap_ref *pRef);

/** Cumulative successful spawns (not live count). */
u32 process_spawn_count(void);

/** Cumulative failed process_spawn attempts. */
u32 process_spawn_fail_count(void);

/** Occupied fixed spawn-table slots (live children not yet reaped). */
u32 process_spawn_live_count(void);

/** Cumulative process_kill calls that resolved a PROCESS cap. */
u32 process_spawn_kill_count(void);

/** Cumulative process_wait reaps. */
u32 process_spawn_wait_count(void);

/** Cumulative PROCESS task-cap mint installs that returned GJ_OK. */
u32 process_spawn_mint_count(void);

/** Cumulative PROCESS mint install failures. */
u32 process_spawn_mint_fail_count(void);

/** Soft post-mint verify PASS count (type/rights/obj/gen). */
u32 process_spawn_mint_soft_count(void);

/** Soft post-mint verify FAIL count. */
u32 process_spawn_mint_soft_bad_count(void);

/**
 * Snapshot soft spawn stats (ok/fail/live/kill/wait/mint*).
 * pOut may be NULL (no-op, returns GJ_ERR_INVAL).
 */
gj_status_t process_spawn_stats(struct gj_spawn_stats *pOut);

/**
 * Greppable soft stats line: "spawn: soft stats ..."
 * Returns live slot count (handy for smoke without parsing).
 */
u32 process_spawn_stats_soft(void);

/**
 * Kill child process (DESTROY right). Calls process_death (G-PROC-5).
 * Idempotent if already dead.
 */
gj_status_t process_kill(struct gj_process *pParent, const struct gj_cap_ref *pRef,
                         u32 u32ExitCode);

/**
 * Wait for child death (WAIT right). Returns exit code or GJ_ERR_AGAIN if live.
 * On success: invalidates parent PROCESS cap and recycles fixed spawn slot.
 */
gj_status_t process_wait(struct gj_process *pParent, const struct gj_cap_ref *pRef,
                         u32 *pOutExit);
