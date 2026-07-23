/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Per-CPU state (struct gj_cpu) + soft pool inventory.
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope
 * -------------------------------------------------------------------------
 * x86_64 product path: one published percpu block per logical CPU. GS_BASE
 * (and KERNEL_GS_BASE for swapgs) point at that block so SYSCALL / IRQ soft
 * paths can recover kernel stack and current thread without a global lock.
 *
 * This header is the soft contract for layout and pool observability.
 * Implementation lives in kernel/cpu/cpu.c; asm offsets are consumed by
 * syscall_entry.S and related stubs — treat the leading fields as ABI.
 *
 * Platform profile: docs/X86_64_INTEL_PLATFORM.md P-SMP-1..6, P-ABI-1.
 * Design ceiling: GJ_MAX_CPUS / GJ_CPU_STATIC_MAX in gj/config.h.
 *
 * -------------------------------------------------------------------------
 * Layout contract (asm offsets — do not reorder leading fields)
 * -------------------------------------------------------------------------
 *   +0  u64KernelRsp   kernel/syscall stack top (gs:0 on SYSCALL entry)
 *   +8  u64UserRsp     user RSP snapshot (swapgs / sysret path)
 *   +16 u64UserRip     user RIP snapshot
 *   +24 u64UserRflags  user RFLAGS snapshot
 *   +32 pCurrent       scheduler current thread (struct gj_thread *)
 *   +40 u64Cr3         active address-space CR3 shadow
 * Remaining fields (u32CpuId, u32Online, aSyscallStack[]) may grow only
 * after this prefix; never insert between +0 and +40.
 *
 * Percpu pool
 * -----------
 *   Static BSS  [0 .. GJ_CPU_STATIC_MAX)     — always linked in image
 *   PMM growth  [STATIC_MAX .. GJ_MAX_CPUS)  — cpu_init_ap allocates dyn
 *
 * Soft inventory (boot telemetry / probes — not hot-path locks)
 * ------------------------------------------------------------
 *   cpu_for_id / cpu_slot_online / cpu_dyn_percpu_count
 *   cpu_soft_* snapshot, kind, counters, greppable cpu_soft_log
 *
 * Soft never hard-gates product bring-up. Counters wrap OK.
 *
 * greppable: cpu: soft
 * greppable: GJ_CPU_SOFT_KIND_
 * greppable: cpu_soft_log / cpu_soft_snapshot / cpu_for_id
 * greppable: GS_BASE KERNEL_GS swapgs
 */
#pragma once

#include <gj/config.h>
#include <gj/types.h>

struct gj_thread;
struct gj_process;

/**
 * Per-CPU fallback stack size (bytes).
 * Used as BSP stack before a thread installs its own kstack; hard IRQs use
 * TSS.RSP0 (see gj/gdt.h), not this array, once the TSS is live.
 */
#define GJ_SYSCALL_STACK_SIZE (16u * 1024u)

/* Soft percpu publish class for a slot (derived + sticky counters). */
#define GJ_CPU_SOFT_KIND_NONE    0u /* OOB or never published online */
#define GJ_CPU_SOFT_KIND_STATIC  1u /* BSS g_aCpus[] online */
#define GJ_CPU_SOFT_KIND_DYN     2u /* PMM-backed g_apCpuDyn[] online */

/**
 * Soft snapshot of the percpu pool (static BSS + PMM dyn growth).
 * Filled by cpu_soft_snapshot(); safe for BSP telemetry after cpu_init_bsp.
 * All sticky counters are wrap-OK diagnostics — never used as hard gates.
 */
struct gj_cpu_soft {
    u32 u32Online;        /* published online count (g_u32NOnline) */
    u32 u32StaticOnline;  /* online slots in [0, GJ_CPU_STATIC_MAX) */
    u32 u32DynOnline;     /* online slots in [STATIC_MAX, GJ_MAX_CPUS) */
    u32 u32DynAlloc;      /* successful PMM percpu allocations */
    u32 u32Oom;           /* PMM percpu alloc failures (sticky) */
    u32 u32Reject;        /* cpu_init_ap OOB / BSP-id rejects (sticky) */
    u32 u32Idempotent;    /* already-online early returns (sticky) */
    u32 u32MaxOnlineId;   /* highest published cpu id (0 if BSP only) */
    u32 u32StaticMax;     /* GJ_CPU_STATIC_MAX (design) */
    u32 u32MaxCpus;       /* GJ_MAX_CPUS (design ceiling) */
    u32 u32LastInitId;    /* last cpu_init_ap id seen (0 after BSP only) */
    u32 u32LastPages;     /* page count of last dyn alloc attempt */
    u64 u64LastDynPa;     /* PA of last successful dyn alloc (0 if none) */
    u32 u32PercpuBytes;   /* sizeof(struct gj_cpu) */
    u32 u32GsSane;        /* soft: 1 if current GS_BASE is a published slot */
};

/**
 * Per-CPU control block. One published instance per online logical CPU.
 *
 * Leading six fields are an asm/SYSCALL contract — see file banner.
 * aSyscallStack is the early/fallback stack body; after a thread is current,
 * u64KernelRsp is typically retargeted to that thread's kernel stack top.
 */
struct gj_cpu {
    /* Must stay at start — asm offsets (syscall_entry.S / swapgs path). */
    u64                 u64KernelRsp;   /* +0  top of per-CPU syscall/kernel stack */
    u64                 u64UserRsp;     /* +8 */
    u64                 u64UserRip;     /* +16 */
    u64                 u64UserRflags;  /* +24 */
    struct gj_thread   *pCurrent;       /* +32 */
    u64                 u64Cr3;         /* +40 current CR3 */
    u32                 u32CpuId;
    u32                 u32Online;      /* 1 once GS published for this slot */
    u8                  aSyscallStack[GJ_SYSCALL_STACK_SIZE]
        __attribute__((aligned(16)));
};

/**
 * Publish BSP slot 0: zero/fill g_aCpus[0], set u64KernelRsp to stack top,
 * mark online, program GS_BASE via cpu_gs_init. Call once on the boot CPU
 * before SYSCALL MSRs or AP bring-up depend on cpu_current().
 */
void cpu_init_bsp(void);

/**
 * Online secondary CPU slot after the AP has reached long mode.
 * Idempotent: already-online slots soft-count and return. May PMM-allocate
 * a dyn percpu block when u32CpuId >= GJ_CPU_STATIC_MAX.
 */
void cpu_init_ap(u32 u32CpuId);

/**
 * Hot path: return the percpu block for the executing CPU (via GS_BASE).
 * Undefined before cpu_gs_init / cpu_init_bsp on that CPU.
 */
struct gj_cpu *cpu_current(void);

/** Hot path: logical cpu id of the executing CPU (u32CpuId). */
u32  cpu_id(void);

/** Published online inventory count (BSP + APs that completed cpu_init_ap). */
u32  cpu_online_count(void);

/**
 * Soft inventory: published percpu for slot, or NULL if OOB / not online.
 * Safe for BSP telemetry; not a substitute for cpu_current() on the hot path.
 */
struct gj_cpu *cpu_for_id(u32 u32CpuId);

/** Soft: 1 if slot has published online percpu (static or PMM-dyn). */
int  cpu_slot_online(u32 u32CpuId);

/** Soft: count of successful PMM-backed percpu allocations (id >= STATIC_MAX). */
u32  cpu_dyn_percpu_count(void);

/* ------------------------------------------------------------------ */
/* Soft percpu pool observability (boot telemetry — not hot-path)       */
/* ------------------------------------------------------------------ */

/** Soft snapshot of pool / counter inventory into *pOut (NULL-safe). */
void cpu_soft_snapshot(struct gj_cpu_soft *pOut);

/**
 * Soft: publish class for slot (GJ_CPU_SOFT_KIND_*).
 * NONE if OOB or not online; STATIC / DYN when published.
 */
u32  cpu_soft_kind(u32 u32CpuId);

/** Soft: count of online slots in the static BSS pool. */
u32  cpu_soft_static_online(void);

/** Soft: count of online PMM-dyn slots (id >= STATIC_MAX). */
u32  cpu_soft_dyn_online(void);

/** Soft: sticky PMM percpu OOM fail count. */
u32  cpu_soft_oom_count(void);

/** Soft: highest published online cpu id (0 when only BSP). */
u32  cpu_soft_max_online_id(void);

/** Soft: kstack top (u64KernelRsp) for published slot, or 0. */
u64  cpu_soft_kstack_top(u32 u32CpuId);

/** Soft: CR3 shadow for published slot, or 0 if unpublished. */
u64  cpu_soft_cr3(u32 u32CpuId);

/**
 * Soft: 1 if MSR_GS_BASE points at a published online percpu (static or dyn).
 * BSP before GS init may report 0; after cpu_init_bsp should be 1 on BSP.
 */
int  cpu_soft_gs_sane(void);

/**
 * Greppable percpu soft summary. Emits:
 *   cpu: soft PASS|UP|PARTIAL online=… static=… dyn=… oom=… …
 *   cpu: soft slot=… kind=… kstack=… cr3=… thr=…
 * Safe any time after cpu_init_bsp; also invoked from BSP/AP soft paths.
 * greppable: cpu: soft
 */
void cpu_soft_log(void);

/** Install pThr as this CPU's pCurrent (scheduler / context switch). */
void cpu_set_current_thread(struct gj_thread *pThr);

/** Return pCurrent for this CPU, or NULL if none / pre-sched. */
struct gj_thread *cpu_current_thread(void);

/**
 * Write CR3 and update the percpu CR3 shadow (u64Cr3).
 * Used on address-space switch; invlpg/shootdown is caller's concern.
 */
void cpu_load_cr3(u64 u64Cr3);

/** Read live CR3 from the CPU (not only the soft shadow). */
u64  cpu_read_cr3(void);

/**
 * Program IA32_GS_BASE (and related) so GS points at *pCpu.
 * Required before cpu_current() / SYSCALL entry can use gs: offsets.
 */
void cpu_gs_init(struct gj_cpu *pCpu);
