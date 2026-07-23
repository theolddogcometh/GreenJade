/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Hybrid Option C: classify + hot handlers + cold personality hook.
 * Pure C11 clean-room — dual MIT OR Apache-2.0.
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/process.h>
#include <gj/types.h>

/*
 * Sparse index table for Linux NR → path/hot fn.
 * Must cover highest NR we set_hot/set_cold (currently ≤ 456).
 */
#define GJ_LINUX_NR_TABLE 512

/**
 * Cold-path personality hook (userspace later: IPC to libprotonrt).
 * Return Linux-style i64 (negative -errno). Called only for COLD path.
 */
typedef i64 (*gj_linux_cold_fn_t)(struct gj_linux_regs *pRegs, void *pCtx);

void gj_linux_dispatch_init(void);

/** Bind current process context for getpid/credentials (until full task). */
void gj_linux_set_current(struct gj_process *pProc, u32 u32Pid, u32 u32Tid);

/* Current process for hot paths (CapJit, etc.). */
extern struct gj_process *g_pLinuxProc;

/** Register cold-path handler; NULL = ENOSYS on cold. */
void gj_linux_set_cold_handler(gj_linux_cold_fn_t pfn, void *pCtx);

/** Attach libprotonrt cold service via cold_ipc queue. */
void gj_protonrt_attach_cold(void);

/** Classify NR → HOT / COLD / NONE. */
enum gj_linux_path gj_linux_classify(u64 u64Nr);

/** Full hybrid dispatch (hot or cold). */
void gj_linux_syscall_dispatch(struct gj_linux_regs *pRegs);

/**
 * Deep NR-table classification + set_hot/set_cold coverage inventory.
 *
 * Slot fields (u32HotSlots/Cold/None/…) are filled by a post-init scan of
 * the path table (authoritative). set_* Ok/Reject count registration
 * attempts. Runtime u64* deepen hybrid accounting beyond hit totals
 * (hot-defer→cold, OOR, PATH_NONE).
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
    /* Runtime path accounting (deepen; mirrors + extends hit stats). */
    u64 u64HotHits;
    u64 u64ColdHits;
    u64 u64Enosys;
    u64 u64HotDeferCold; /* HOT handler returned -ENOSYS → cold/ENOSYS */
    u64 u64Oor;          /* NR >= table */
    u64 u64NonePath;     /* in-table PATH_NONE → ENOSYS */
};

/** Snapshot classification + coverage + runtime counters. */
void gj_linux_nr_class_stats_get(struct gj_linux_nr_class_stats *pOut);

/**
 * Soft inventory log (greppable):
 *   linux: nr class soft PASS|PARTIAL|NONE hot=… cold=… none=… class=…
 *          max=… table=… set_hot=… set_cold=… rej_h=… rej_c=…
 *          hits_h=… hits_c=… enosys=… defer=… oor=… none_path=…
 */
void gj_linux_nr_class_soft_log(void);

/* Hot path implementations (linux_hot.c) */
i64 gj_linux_hot_write(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_yield(struct gj_linux_regs *pRegs);
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
i64 gj_linux_hot_mlock(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_munlock(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mlockall(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_munlockall(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pkey_alloc(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pkey_free(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getppid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getsid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_personality(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_exit(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_exit_group(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_brk(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mmap(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_munmap(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mremap(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mprotect(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_gettime(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_getres(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_nanosleep(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_nanosleep(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_futex(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_futex_wake2(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_futex_wait2(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_arch_prctl(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_set_tid_address(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getgroups(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpgrp(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sysinfo(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_times(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getrusage(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpriority(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setpriority(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_uname(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getrandom(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pread64(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pwrite64(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_writev(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_readv(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_preadv(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pwritev(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_tkill(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_tgkill(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sigaltstack(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getscheduler(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setscheduler(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getparam(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setparam(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_get_priority_max(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_get_priority_min(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_msync(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mincore(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getsockopt(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setsockopt(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getsockname(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getpeername(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_shutdown(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rt_sigaction(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rt_sigprocmask(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rt_sigreturn(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getitimer(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_setitimer(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_alarm(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_pause(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_prctl(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_set_robust_list(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_get_robust_list(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_waitid(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_process_vm_readv(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_process_vm_writev(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_membarrier(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_rseq(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setaffinity(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getaffinity(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_capget(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_capset(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_getcpu(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_gettimeofday(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_settimeofday(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_time(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_settime(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_clock_adjtime(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_setattr(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_sched_getattr(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_mbind(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_set_mempolicy(struct gj_linux_regs *pRegs);
i64 gj_linux_hot_get_mempolicy(struct gj_linux_regs *pRegs);
