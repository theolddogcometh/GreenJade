/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * posix_spawn-like process create + PROCESS task cap (G-PROC-*).
 * Product lifecycle: spawn → kill (process_death) → wait (reap + recycle slot).
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

struct gj_process;
struct gj_cap_ref;

#define GJ_SPAWN_CNODE_SLOTS 64ull
#define GJ_SPAWN_MAX         8u

struct gj_spawn_args {
    void              (*pfnEntry)(void *pArg);
    void               *pArg;
    u32                 u32Jit;         /* CapJit for child */
    u32                 u32Personality; /* 0 native, 1 linux */
};

/**
 * Create child process: CNode, root meta, private AS, first thread.
 * Mints GJ_CAP_PROCESS into parent CNode (G-PROC-2).
 * Wait-registers child so process_death reclaims AS/CNode (G-PROC-5).
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
