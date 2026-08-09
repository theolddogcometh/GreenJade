/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * posix_spawn-like process create + PROCESS task cap (G-PROC-*).
 * Pure C11 freestanding; dual MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Product process lifecycle for GreenJade:
 *   spawn -> kill (process_death) -> wait (reap + recycle slot)
 *
 * On spawn the parent receives a minted GJ_CAP_PROCESS in its CNode (Scheme A
 * handle: slot + gen). That cap is the transferable task port; root meta in
 * slot 0 is NOT a factory for PROCESS caps (docs/CAP_ADDRESSING.md K1-K6).
 *
 * Lean residual role (Soft!=product; G-AC-1; H3)
 * -----------------------------------------------
 * Fixed-table process_spawn is the lean residual path for starting
 * userspace UDX / driver hosts (confined C processes; DDI caps later):
 *   ddi_host_gj   — soft DDI door residual host (GJ_SYS_DDI)
 *   rtl8168_udx   — NIC class host (10ec:8168; Dual DoD B direction)
 *   xhci_udx      — USB HC host (8086:a12f; Dual DoD A direction)
 * Soft residual != product multi-server confine / UDX host product close.
 * G-AC-1: no Linux .ko binary runs in the kernel as product.
 * H3: process_kill -> process_death (thr_exit before as_destroy) preserved.
 * Launch residual path (soft honesty only; product host launch OPEN):
 *   process_spawn -> G-PROC-2 mint PROCESS -> first thr -> wait_register
 *   -> host runs (native personality preferred) -> DDI/UDX ops later
 *   -> process_kill / thr_exit -> G-PROC-5 death (H3) -> process_wait reap
 * Concurrent: GJ_SPAWN_HOST_CLASS_N (3) named hosts fit in GJ_SPAWN_MAX.
 * greppable: "spawn: soft residual" / "spawn: soft residual lean"
 * greppable: "spawn: soft residual lean host" / "spawn: soft host_launch"
 * greppable: "spawn: soft residual host_catalog"
 * greppable: "spawn: soft residual host_path"
 * greppable: "spawn: soft residual dual_dod"
 * greppable: "spawn: soft host_launch lean"
 *
 * Product lifecycle (G-PROC-*)
 * ----------------------------
 *   G-PROC-2  mint PROCESS into parent with base rights (+ JIT when set)
 *   G-PROC-5  process_death reclaims AS/CNode; wait recycles fixed slot
 *   Soft post-mint verify: type / rights / obj / gen (fail counted, not ABI)
 *   H3 death residual: kill path calls process_death (siblings EXIT first)
 *
 * Soft product surface
 * --------------------
 *   greppable: "spawn: soft stats"
 *   greppable: "spawn: mint soft"
 *   greppable: "spawn: soft residual lean"
 *   greppable: "spawn: soft residual lean host"
 *   greppable: "spawn: soft residual host_catalog"
 *   greppable: "spawn: soft residual host_path"
 *   greppable: "spawn: soft residual dual_dod"
 *   greppable: "spawn: soft host_launch"
 *   greppable: "spawn: soft host_launch lean"
 *   PROCESS cap mint verify (type/rights/obj/gen) after parent install
 *   Cumulative + live + fail/kill/wait/mint counters
 *
 * Linux pid fork/wait product-min (process.h / process.c - not this file)
 * ----------------------------------------------------------------------
 *   greppable: "process: soft fork-wait product-min"
 *   process_fork_soft / process_clone_soft / process_wait_soft /
 *   process_waitid_soft - PCB parent -> child pid + wait4/waitid reaper.
 *   Cap lifecycle here remains process_spawn / process_kill / process_wait.
 *   Soft!=product full posix_spawn; cold personality wires later.
 *
 * Failure policy
 * --------------
 * Any failure after partial install reverse tear-downs (no orphan AS/cap).
 * Fixed table GJ_SPAWN_MAX; full table -> spawn fail (soft u32Fail).
 *
 * Related: gj/process.h, gj/cap.h, gj/thread.h, gj/elf_load.h, gj/user_task.h
 * docs/CAP_ADDRESSING.md / docs/APPLE_CHANNEL_REMAINING.md
 * docs/LAPTOP_LINUX_DRIVER_HOST.md / docs/DDI_SOFT.md
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

/** Child CNode slot count at spawn (bring-up size; not hierarchical quota). */
#define GJ_SPAWN_CNODE_SLOTS 64ull
/** Fixed spawn-table capacity (live children not yet reaped). */
#define GJ_SPAWN_MAX         8u

/*
 * Named userspace UDX / driver host catalog (launch residual; Soft!=product).
 * process_spawn starts confined C hosts; DDI MMIO/IRQ/DMA cap mint remains
 * OPEN (not this unit). G-AC-1: userspace hosts only — never in-kernel .ko.
 * Dual DoD A = xhci_udx; Dual DoD B = rtl8168_udx. ddi_host_gj = DDI residual.
 * Concurrent class hosts must fit fixed table: HOST_CLASS_N <= SPAWN_MAX.
 */
#define GJ_SPAWN_HOST_CLASS_N            3u
#define GJ_SPAWN_HOST_DDI_NAME           "ddi_host_gj"
#define GJ_SPAWN_HOST_RTL_NAME           "rtl8168_udx"
#define GJ_SPAWN_HOST_XHCI_NAME          "xhci_udx"
/** Realtek 8168 NIC class (Dual DoD B direction). Soft catalog only. */
#define GJ_SPAWN_HOST_RTL_VEND           0x10ecu
#define GJ_SPAWN_HOST_RTL_DEV            0x8168u
/** Intel xHCI HC class (Dual DoD A direction; G752 soft 8086:a12f). */
#define GJ_SPAWN_HOST_XHCI_VEND          0x8086u
#define GJ_SPAWN_HOST_XHCI_DEV           0xa12fu
/** Preferred personality for UDX driver hosts (0 = native GJ_SYS_*). */
#define GJ_SPAWN_HOST_PERSONA_NATIVE     0u
/**
 * Soft self-check budget for host-launch residual (geometry + honesty).
 * ok/GJ_SPAWN_HOST_LAUNCH_OK_N — never hard-gates product.
 */
#define GJ_SPAWN_HOST_LAUNCH_OK_N        8u

/* Default PROCESS task rights (G-PROC-2); JIT is OR'd when CapJit is set. */
#define GJ_SPAWN_PROCESS_RIGHTS_BASE                                       \
    ((u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY | GJ_RIGHT_DESTROY |          \
           GJ_RIGHT_WAIT | GJ_RIGHT_VM | GJ_RIGHT_SPAWN))

/* Core task rights soft-required after mint (kill/reap/vm surface). */
#define GJ_SPAWN_PROCESS_RIGHTS_CORE                                       \
    ((u16)(GJ_RIGHT_DESTROY | GJ_RIGHT_WAIT | GJ_RIGHT_VM))

/**
 * Spawn arguments for the first child thread / personality.
 * pfnEntry may be a kernel thr entry or paired with user_task maps later.
 * u32Personality: 0 native (GJ_SYS_*), 1 linux (Option C hybrid).
 */
struct gj_spawn_args {
    void              (*pfnEntry)(void *pArg);
    void               *pArg;
    u32                 u32Jit;         /* CapJit for child */
    u32                 u32Personality; /* 0 native, 1 linux */
};

/**
 * Soft spawn / PROCESS-mint counters (observability; not a hard ABI).
 * Grep: spawn: soft stats
 *
 * u32Live is occupied fixed slots (not cumulative ok).
 * Mint soft PASS/FAIL is post-install verify only (install already returned).
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

/**
 * Look up child by PROCESS cap in parent CNode.
 * Fail closed on type/gen mismatch (returns NULL; no stale object use).
 */
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
 * Kill child process (DESTROY right). Calls process_death (G-PROC-5 + H3).
 * H3: death exits non-current siblings before as_destroy (Soft!=product).
 * Idempotent if already dead. Requires parent ownership of the PROCESS cap.
 * UDX driver hosts (ddi_host_gj / rtl8168_udx / xhci_udx) reaped via this path.
 * Host teardown residual: thr_exit barrier before private AS destroy (H3).
 * greppable: spawn: kill ... H3=death_residual
 */
gj_status_t process_kill(struct gj_process *pParent, const struct gj_cap_ref *pRef,
                         u32 u32ExitCode);

/**
 * Wait for child death (WAIT right). Returns exit code or GJ_ERR_AGAIN if live
 * (poll-friendly, like wait4 WNOHANG with live child).
 * On success: invalidates parent PROCESS cap, forgets wait-table slot, recycles
 * fixed spawn slot. Linux pid wait uses process_wait4* (process: soft wait).
 * Host-launch residual: AGAIN + yield so concurrent UDX host thr can run.
 * greppable: spawn: soft wait
 * greppable: spawn: soft wait poll
 */
gj_status_t process_wait(struct gj_process *pParent, const struct gj_cap_ref *pRef,
                         u32 *pOutExit);
