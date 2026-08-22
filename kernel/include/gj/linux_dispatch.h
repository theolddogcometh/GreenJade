/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Hybrid Option C: classify + hot handlers + cold personality hook.
 * Pure C11 clean-room - dual MIT OR Apache-2.0. No Linux/GPL source.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Classify each Linux NR as HOT (entirely in kernel), COLD (personality
 * upcall via cold_ipc / doors), or NONE (ENOSYS). Hot handlers live in
 * linux_hot.c; cold path is libprotonrt-shaped (docs/PROTON_PERSONALITY.md).
 *
 * Architecture (docs/LINUX_ABI_HYBRID.md)
 * --------------------------------------
 *   ring-3 Linux task
 *        │ SYSCALL
 *        ▼
 *   gj_linux_syscall_dispatch
 *        ├─ HOT  -> gj_linux_hot_* (+ user_copy, CapJit, PA futex)
 *        ├─ COLD -> cold_ipc_submit -> doors / service / queue
 *        └─ NONE -> -ENOSYS
 *
 * -------------------------------------------------------------------------
 * Lean residual (this header + linux_dispatch.c only; Soft!=product)
 * -------------------------------------------------------------------------
 * Exclusive residual deepen is the dispatch-table cold/hot split and the
 * product personality route honesty - NOT freestanding rtl / UDX thrash.
 * Soft!=product. G-AC-1 (no Linux .ko product AC). Dual MIT OR Apache-2.0.
 * No version stamp. No stamp storms. Never hard-gates hybrid product.
 *
 * Cold/hot split honesty (soft once-lamp residual; deepen):
 *   PATH_HOT  = in-kernel gj_linux_hot_* (may soft-defer -ENOSYS -> cold)
 *   PATH_COLD = personality (cold_ipc doors/service/queue, or legacy hook)
 *   PATH_NONE = -ENOSYS (in-table unregistered or OOR as NONE-shaped)
 *   Static claim: hot_slots + cold_slots + none_slots (+ path_bad) == table
 *   Armed honesty: armed_ok when every PATH_HOT has non-NULL pfn
 *   split_ok when hybrid both sides live, armed_ok, path_bad==0
 *   Runtime path split tallies: hot_direct | hot_defer | hot_null |
 *     cold_ipc | cold_leg | cold_bare | path_none | oor
 *   Soft bp: table slot share + runtime path share (basis points; soft only)
 *   hot_may_defer=1 (HOT -ENOSYS falls through to personality)
 *   freestanding_thrash=0 (dispatch residual only - not rtl/UDX wire)
 *   greppable: linux: dispatch soft residual cold_hot split
 *
 * Product personality residual (soft once-lamp residual; deepen):
 *   Cold NRs are libprotonrt-shaped (docs/PROTON_PERSONALITY.md).
 *   Prefer cold_ipc_personality_attached() (doors/service) over legacy
 *   g_pfnCold; bare ENOSYS when neither bound.
 *   pers_route: 2=attached preferred, 1=legacy only, 0=bare
 *   pers_prefer_ok=1 when attached is preferred route (product shape)
 *   cold_route bp: ipc | leg | bare share of cold hits (soft only)
 *   init_module / delete_module / finit_module stay PATH_COLD eng residual
 *   only - never product .ko AC (G-AC-1). freestanding_thrash=0.
 *   greppable: linux: dispatch soft residual personality
 *
 * Shared lean residual rollup:
 *   greppable: linux: dispatch soft residual lean
 *   greppable: linux: soft residual lean
 *   greppable: Soft!=product | soft_ne_product=1 | G-AC-1 | freestanding_thrash=0
 *
 * Soft product surfaces
 * ---------------------
 *   greppable: "linux: nr class soft"
 *   greppable: "linux: dispatch soft"
 *   greppable: GJ_LINUX_NR_TABLE
 *   gj_linux_nr_class_stats - table inventory + runtime deepen
 *   gj_linux_nr_class_soft_log - safe pre-init (idle NONE); PASS needs integrity
 *   Coarse hits also mirrored on gj_linux_dispatch_stats (syscall.h)
 *
 * Registration contract
 * ---------------------
 *   set_hot / set_cold fill a sparse table of size GJ_LINUX_NR_TABLE.
 *   Must cover the highest NR registered (currently ≤ 456).
 *   OOR or NULL hot pfn -> reject (soft-counted); cold may be NULL -> ENOSYS.
 *
 * Related: gj/linux_abi.h (NRs, path enum, regs), gj/cold_ipc.h,
 *          gj/syscall.h (top-level route), gj/process.h (current PCB),
 *          docs/LINUX_ABI_HYBRID.md, docs/PROTON_PERSONALITY.md.
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/process.h>
#include <gj/types.h>

/*
 * Sparse index table for Linux NR -> path/hot fn.
 * Must cover highest NR we set_hot/set_cold (currently ≤ 456).
 * Slots default PATH_NONE until init registers the product hot/cold set.
 */
#define GJ_LINUX_NR_TABLE 512

/**
 * Cold-path personality hook (userspace later: IPC to libprotonrt).
 * Return Linux-style i64 (negative -errno). Called only for COLD path.
 * pCtx is the cookie from gj_linux_set_cold_handler / service bind.
 */
typedef i64 (*gj_linux_cold_fn_t)(struct gj_linux_regs *pRegs, void *pCtx);

/**
 * Build NR path table + default hot set. Call from gj_syscall_init path.
 * Soft inventory is available after this via nr_class_stats / soft_log.
 */
void gj_linux_dispatch_init(void);

/**
 * Bind current process context for getpid/credentials (until full task).
 * Hot paths that need PCB/cred read g_pLinuxProc and the soft pid/tid.
 */
void gj_linux_set_current(struct gj_process *pProc, u32 u32Pid, u32 u32Tid);

/**
 * Current process for hot paths (CapJit, mmap AS, credentials).
 * May be NULL in early smoke; handlers must fail closed on required PCB.
 */
extern struct gj_process *g_pLinuxProc;

/** Register cold-path handler; NULL = ENOSYS on cold. */
void gj_linux_set_cold_handler(gj_linux_cold_fn_t pfn, void *pCtx);

/**
 * Attach libprotonrt cold service via cold_ipc queue / service registration.
 * Product bring-up: wires Option C cold path to in-tree protonrt.
 */
void gj_protonrt_attach_cold(void);

/**
 * Classify NR -> HOT / COLD / NONE (gj_linux_path from linux_abi.h).
 * OOR NRs behave as NONE for dispatch (ENOSYS + soft u64Oor).
 */
enum gj_linux_path gj_linux_classify(u64 u64Nr);

/**
 * Full hybrid dispatch (hot or cold).
 * Writes pRegs->i64Ret. NULL pRegs is a defensive no-op (if implemented).
 * Hot returning -ENOSYS may soft-defer to cold (u64HotDeferCold).
 */
void gj_linux_syscall_dispatch(struct gj_linux_regs *pRegs);

/**
 * Deep NR-table classification + set_hot/set_cold coverage inventory.
 *
 * Slot fields (u32HotSlots/Cold/None/...) are filled by a post-init scan of
 * the path table (authoritative). set_* Ok/Reject count registration
 * attempts. Runtime u64* deepen hybrid accounting beyond hit totals
 * (hot-defer->cold, OOR, PATH_NONE, null guard, cold route split).
 *
 * Cold/hot split honesty (Soft!=product; residual only; deepen):
 *   u32HotSlots / u32ColdSlots / u32NoneSlots = static table path claim.
 *   u64HotHits  = HOT direct success (handler ret != -ENOSYS).
 *   u64HotDeferCold / u64HotNull = HOT that fell through to cold/ENOSYS.
 *   u64ColdHits + u64ColdIpc / u64ColdLegacy / u64ColdBare =
 *     personality route split (attached doors/service vs legacy vs bare).
 *   Soft residual once-lamps never rewrite these counters or hard-gate.
 *   Residual deepen emits soft bp shares + sum_ok / split_ok / prefer_ok
 *   honesty only - freestanding_thrash=0 (no rtl/UDX wire thrash).
 *
 * Safety: GJ_LINUX_PATH_HOT == 0, so BSS path[] looks fully HOT before
 * gj_linux_dispatch_init. soft_log / stats_get must not treat pre-init
 * tables as product coverage (g_fNrClassLive / idle NONE path).
 *
 * Soft log: gj_linux_nr_class_soft_log (greppable PASS|PARTIAL|NONE).
 * Residual: linux: dispatch soft residual lean|cold_hot split|personality.
 * Never hard-gates product paths; wrap-OK diagnostics only.
 * Soft!=product · G-AC-1 · dual MIT OR Apache-2.0 · freestanding_thrash=0.
 */
struct gj_linux_nr_class_stats {
    u32 u32TableSize;    /* GJ_LINUX_NR_TABLE */
    u32 u32HotSlots;     /* PATH_HOT slots */
    u32 u32ColdSlots;    /* PATH_COLD slots */
    u32 u32NoneSlots;    /* PATH_NONE slots */
    u32 u32Classified;   /* hot + cold */
    u32 u32MaxHotNr;     /* highest NR set_hot accepted */
    u32 u32MaxColdNr;    /* highest NR set_cold accepted */
    u32 u32MaxClassNr;   /* max(hot, cold) NR */
    u32 u32SetHotOk;     /* successful set_hot */
    u32 u32SetColdOk;    /* successful set_cold */
    u32 u32SetHotReject; /* OOR or NULL pfn */
    u32 u32SetColdReject;/* OOR */
    /* Scan integrity (soft deepen; post-init authoritative). */
    u32 u32HotArmed;     /* PATH_HOT with non-NULL pfn */
    u32 u32HotNullSlots; /* PATH_HOT with NULL pfn (table inconsistency) */
    u32 u32PathBad;      /* path byte not HOT/COLD/NONE */
    u32 u32Live;         /* 1 after successful dispatch_init scan */
    /* Runtime path accounting (deepen; mirrors + extends hit stats). */
    u64 u64HotHits;
    u64 u64ColdHits;
    u64 u64Enosys;
    u64 u64HotDeferCold; /* HOT handler returned -ENOSYS -> cold/ENOSYS */
    u64 u64HotNull;      /* PATH_HOT runtime with NULL pfn -> cold/ENOSYS */
    u64 u64Oor;          /* NR >= table */
    u64 u64NonePath;     /* in-table PATH_NONE -> ENOSYS */
    u64 u64Entries;      /* non-NULL dispatch calls */
    u64 u64NullGuard;    /* NULL pRegs early return */
    u64 u64ColdIpc;      /* cold_ipc_submit hits */
    u64 u64ColdLegacy;   /* g_pfnCold legacy hook hits */
    u64 u64ColdBare;     /* cold/hot-defer with no personality -> ENOSYS */
    u64 u64LastNr;       /* last NR observed (soft snapshot) */
    u64 u64LastRet;      /* last i64Ret bit pattern */
};

/**
 * Snapshot classification + coverage + runtime counters.
 * NULL pOut -> no-op. Pre-init: returns zeroed inventory (no HOT=0 BSS trap).
 */
void gj_linux_nr_class_stats_get(struct gj_linux_nr_class_stats *pOut);

/**
 * Soft inventory log (greppable; safe pre-init -> idle NONE):
 *   linux: nr class soft PASS|PARTIAL|NONE hot=... cold=... none=... class=...
 *          max=... table=... set_hot=... set_cold=... rej_h=... rej_c=...
 *   linux: nr class soft hits_h=... hits_c=... enosys=... defer=... hot_null=...
 *          oor=... none_path=... max_h=... max_c=...
 *   linux: nr class soft armed=... null_slots=... path_bad=... live=...
 *          entries=... null=... cold_ipc=... cold_leg=... cold_bare=...
 *          last_nr=... last_ret=...
 *   linux: nr class soft idle (dispatch not init)   - pre-init only
 */
void gj_linux_nr_class_soft_log(void);

/* -------------------------------------------------------------------------
 * Hot path implementations (linux_hot.c)
 * -------------------------------------------------------------------------
 * Each returns Linux-style i64 in the hot convention (negative -errno).
 * Handlers must copy user memory with fail-closed FAULT -> -EFAULT.
 * CapJit / W|X policy is enforced on mprotect-class ops from PCB flags.
 * ------------------------------------------------------------------------- */

/* I/O and yield */
i64 gj_linux_hot_write(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_yield(struct gj_linux_regs *pRegs);

/* Identity / credentials */
i64 gj_linux_hot_getpid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_gettid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getuid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getgid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_geteuid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getegid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setuid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setgid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setreuid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setregid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setresuid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setresgid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getresuid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getresgid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setpgid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setsid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setgroups(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getgroups(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpgrp(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpgid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getppid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getsid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_umask(struct gj_linux_regs *pRegs);

/* Memory locking / pkeys (soft product) */
i64 gj_linux_hot_mlock(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_munlock(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mlockall(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_munlockall(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pkey_alloc(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pkey_free(struct gj_linux_regs *pRegs);

/* Process control / VM */
i64 gj_linux_hot_personality(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_exit(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_exit_group(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_brk(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mmap(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_munmap(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mremap(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mprotect(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_msync(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mincore(struct gj_linux_regs *pRegs);

/* Time */
i64 gj_linux_hot_clock_gettime(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_getres(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_nanosleep(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_nanosleep(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_gettimeofday(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_settimeofday(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_time(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_settime(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_adjtime(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getitimer(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setitimer(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_alarm(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pause(struct gj_linux_regs *pRegs);

/* Futex / TLS / tid */
i64 gj_linux_hot_futex(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_futex_wake2(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_futex_wait2(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_arch_prctl(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_set_tid_address(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_set_robust_list(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_get_robust_list(struct gj_linux_regs *pRegs);

/* Info / misc */
i64 gj_linux_hot_sysinfo(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_times(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getrusage(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpriority(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setpriority(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_uname(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getrandom(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_prctl(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getcpu(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_capget(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_capset(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_membarrier(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rseq(struct gj_linux_regs *pRegs);

/* Vectored / positioned I/O (hot when fd table allows) */
i64 gj_linux_hot_pread64(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pwrite64(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_writev(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_readv(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_preadv(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pwritev(struct gj_linux_regs *pRegs);

/* Signals (soft subset) */
i64 gj_linux_hot_tkill(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_tgkill(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sigaltstack(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rt_sigaction(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rt_sigprocmask(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rt_sigreturn(struct gj_linux_regs *pRegs);

/* Sched policy / affinity */
i64 gj_linux_hot_sched_getscheduler(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setscheduler(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getparam(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setparam(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_get_priority_max(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_get_priority_min(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setaffinity(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getaffinity(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setattr(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getattr(struct gj_linux_regs *pRegs);

/* Socket gets (hot stubs / soft) */
i64 gj_linux_hot_getsockopt(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setsockopt(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getsockname(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpeername(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_shutdown(struct gj_linux_regs *pRegs);

/* Wait / process_vm / NUMA soft */
i64 gj_linux_hot_waitid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_process_vm_readv(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_process_vm_writev(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mbind(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_set_mempolicy(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_get_mempolicy(struct gj_linux_regs *pRegs);
