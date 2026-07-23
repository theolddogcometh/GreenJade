/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel threads + cooperative scheduler (block/wake/yield).
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Per-thread control blocks, dual stacks, cooperative pick_next, and the
 * hard invariants that keep SYSCALL + IRQ stacks from smashing each other.
 * Used by doors cold path, personality servers, spawn first-thread, PE32
 * user entry, and AP idle loops.
 *
 * Dual stacks per thread
 * ----------------------
 *   aStack  — cooperative switch_context (schedule / trampoline)
 *   aKstack — per-thr SYSCALL stack so door_recv can block mid-call (G-PERS)
 *
 * Hard invariants (prior residual-#UD fix — do not regress)
 * --------------------------------------------------------
 *   1) TSS.RSP0 always the dedicated IRQ stack (tss_use_irq_rsp0). Never
 *      pin RSP0 to a thr aKstack: a parked thr mid-syscall has frames there;
 *      another thr's ring-3 IRQ would smash them → garbage RIP / kernel #UD.
 *   2) Per-CPU GS USER_* (RIP/RSP/RFLAGS) is not per-thread. schedule()
 *      save/restore via u64SysUser* so yield mid-syscall sysretq's to the
 *      correct user context after another thr has run.
 *   3) Install thr SYSCALL kstack (u64KernelRsp) only after switch_context
 *      (or from the user-entry trampoline) — never while still on pCur.
 *
 * Soft product surfaces
 * ---------------------
 *   QoS classes GJ_QOS_* (Apple-shaped pick rank + capped PI boost)
 *   Kstack base + mid canaries + poison high-water soft scan
 *   greppable: "sched: soft stats"
 *   greppable: GJ_QOS_ GJ_THR_F_USER GJ_THR_KSTACK_CANARY
 *
 * Code map
 * --------
 *   kernel/sched/thread.c          — create / block / wake / pick / QoS
 *   kernel/arch/x86_64/switch.S    — switch_context
 *   kernel/cpu/cpu.c               — GS USER_* / u64KernelRsp / TSS.RSP0
 *
 * Related: gj/process.h (pProc owner), gj/spawn.h, gj/user_task.h,
 *          gj/except.h (block tags on exception port).
 * docs/LINUX_ABI_HYBRID.md · docs/APPLE_CHANNEL_REMAINING.md §8 (QoS spirit)
 */
#pragma once

#include <gj/types.h>

struct gj_process;
struct gj_thread;

/**
 * Thread lifecycle states.
 * UNUSED slots may be recycled; EXITED threads stay until reaped by product
 * policy (bring-up may leave them occupied — fixed GJ_MAX_THREADS table).
 */
enum gj_thread_state {
    GJ_THR_UNUSED   = 0,
    GJ_THR_RUNNABLE = 1,
    GJ_THR_RUNNING  = 2,
    GJ_THR_BLOCKED  = 3,
    GJ_THR_EXITED   = 4,
};

/*
 * Stack sizes. 8 KiB aKstack was too small: deep door/net/store paths + trap
 * frames overflowed, corrupted saved RSP frames, and produced kernel #UD with
 * garbage RIP (e.g. 0x510e2002 near store door tokens) while thr=sshd was
 * current. Canary at aKstack[0] catches further growth pressure.
 *
 * aStack remains smaller: pure cooperative switch frames only.
 * Mid canary + poison fill give early overflow soft signals before base smash.
 */
#define GJ_THR_STACK_SIZE    (16u * 1024u)
#define GJ_THR_KSTACK_SIZE   (32u * 1024u)
#define GJ_MAX_THREADS       32
#define GJ_THR_KSTACK_CANARY     0xC4A1C4A1C4A1C4A1ull
/* Mid-stack soft canary: early overflow signal before base canary. */
#define GJ_THR_KSTACK_CANARY_MID 0xA5C3A5C3A5C3A5C3ull
/* 8-byte aligned midpoint slot (grows-down kstack soft depth marker). */
#define GJ_THR_KSTACK_MID        ((GJ_THR_KSTACK_SIZE / 2u) & ~7u)
/* Soft poison fill for high-water soft scan (unused bytes). */
#define GJ_THR_KSTACK_POISON     0xFDu

/*
 * QoS classes (Apple-shaped soft product). 0..2 keep historical meaning
 * used by syscalls / main personality; 3..4 deepen soft desktop classes.
 * Rank (higher first): DRIVER > INTERACTIVE > NORMAL > UTILITY > BACKGROUND.
 * Capped soft PI boost (thread_qos_boost_soft) may temporarily lift rank.
 * Out-of-range class clamps to NORMAL (soft clamp counted).
 */
#define GJ_QOS_NORMAL       0u
#define GJ_QOS_INTERACTIVE  1u
#define GJ_QOS_BACKGROUND   2u
#define GJ_QOS_UTILITY      3u
#define GJ_QOS_DRIVER       4u /* soft-RT UDX/audio; quota-backed later */
#define GJ_QOS_CLASS_MAX    4u
/* Soft boost residual cap (Apple §8 capped PI spirit; no unbounded lift). */
#define GJ_QOS_BOOST_CAP    4u

/** First schedule enters 64-bit ring-3 at u64UserRip / u64UserRsp. */
#define GJ_THR_F_USER_ENTRY   (1u << 0)
/** First schedule enters compat CS32 via iretq (PE32 / WoW64 path). */
#define GJ_THR_F_USER32_ENTRY (1u << 1)

/**
 * Kernel thread control block (fixed table entry; not a heap object).
 *
 * Security / correctness notes:
 *   - u64Cr3: 0 means "use CPU default / boot CR3" (shared AS bring-up).
 *   - pBlockObj + u32BlockTag form the wait key for thread_wake (pointer
 *     identity; no refcount — object must outlive waiters or wake-all first).
 *   - u64SysUser* valid only while mid-SYSCALL (u32SysUserValid != 0).
 *   - aKstack grows down from u64KstackTop; base + mid canaries planted on create.
 */
struct gj_thread {
    u32                 u32Id;
    u32                 u32State;        /* GJ_THR_* */
    struct gj_process  *pProc;
    u64                 u64Cr3;          /* address space; 0 = use cpu default */
    /* Saved kernel context for switch_context (aStack or mid-syscall aKstack) */
    u64                 u64Rsp;
    void               *pBlockObj;       /* futex key / cold request id owner */
    u32                 u32BlockTag;
    u32                 u32Flags;        /* GJ_THR_F_* */
    void              (*pfnEntry)(void *pArg);
    void               *pArg;
    u64                 u64UserRip;      /* first ring-3 entry (G-PERS) */
    u64                 u64UserRsp;
    u64                 u64KstackTop;    /* top of aKstack → cpu u64KernelRsp */
    u64                 u64KstackCanary; /* expected value at aKstack base */
    /*
     * Mid-syscall user return target. Per-CPU GS USER_* is overwritten when
     * another thr runs; schedule() copies these across the switch.
     * u32SysUserValid: 0 = never saved (kernel-only thr / not in SYSCALL).
     */
    u64                 u64SysUserRip;
    u64                 u64SysUserRsp;
    u64                 u64SysUserRflags;
    u32                 u32SysUserValid;
    u8                  aStack[GJ_THR_STACK_SIZE] __attribute__((aligned(16)));
    u8                  aKstack[GJ_THR_KSTACK_SIZE] __attribute__((aligned(16)));
};

/** Initialize BSP thread table + idle. Call once before thread_create. */
void thread_init(void);

/**
 * Create runnable kernel thread. Returns thread id or 0 on failure.
 * pfn may be NULL for user-entry threads (flags / user fields set by other APIs).
 * Failure modes: table full, bad args (implementation soft-counted).
 */
u32 thread_create(struct gj_process *pProc, void (*pfn)(void *), void *pArg);

/**
 * Create a thread that enters ring-3 at u64Entry with stack u64Stack on first
 * schedule (G-PERS userspace personality). Sets GJ_THR_F_USER_ENTRY.
 */
u32 thread_create_user(struct gj_process *pProc, u64 u64Entry, u64 u64Stack);

/**
 * Create a thread that enters 32-bit compat ring-3 (CS32) via iretq.
 * Sets GJ_THR_F_USER32_ENTRY. Used by PE32 / pe32_hw_enter paths.
 */
u32 thread_create_user32(struct gj_process *pProc, u64 u64Entry, u64 u64Stack);

/**
 * After execve: rewrite all user-entry threads of pProc to (entry, stack).
 * Returns count of threads updated. Does not create new threads.
 * Kernel-only threads of pProc are left alone.
 */
u32 thread_exec_replace(struct gj_process *pProc, u64 u64Entry, u64 u64Stack);

/**
 * Query thread state (GJ_THR_*). Returns GJ_THR_UNUSED if id unknown.
 */
u32 thread_get_state(u32 u32Id);

/** Voluntary cooperative yield (becomes RUNNABLE; schedule picks next). */
void thread_yield(void);
/** Soft preemption: set flag; next schedule/idle path yields. */
void thread_yield_request(void);
/** Consume pending yield request; returns 1 if yield was requested. */
int  thread_yield_pending(void);
/** Current thread → EXITED and schedule away (does not return). */
void thread_exit(void);

/**
 * Block current thread (caller must schedule()).
 * pBlockObj + u32Tag form the wake key; see EXCEPT_TAG_* for exception port.
 */
void thread_block(void *pBlockObj, u32 u32Tag);

/**
 * Wake up to u32Max threads blocked on pBlockObj (u32Tag 0 = any tag).
 * Returns number woken. Safe when no waiters (returns 0).
 */
u32  thread_wake(void *pBlockObj, u32 u32Tag, u32 u32Max);

/** Current thread TCB or NULL if called off-thread / before init. */
struct gj_thread *thread_current(void);

/**
 * Cooperative context switch. Saves/restores SYSCALL USER_* on the thr,
 * keeps TSS.RSP0 on the dedicated IRQ stack, installs next thr kstack only
 * after switch_context returns on that thr.
 * Must not be called while holding spinlocks (STYLE / SECURITY: no schedule
 * under spinlock).
 */
void schedule(void);

/**
 * Point per-CPU SYSCALL stack (u64KernelRsp) at pThr->u64KstackTop and
 * reassert TSS.RSP0 = dedicated IRQ stack. Never uses thr kstack as RSP0.
 */
void thread_install_kstack(struct gj_thread *pThr);

/** Enter scheduler forever (idle + others). BSP product path. */
void scheduler_run(void) __attribute__((noreturn));

/**
 * Per-CPU idle + schedule loop for AP (HLT when nothing runnable on this CPU).
 * Called from smp_ap_c_entry after AP online.
 */
void scheduler_run_ap(void) __attribute__((noreturn));

/**
 * Install per-CPU idle for u32Cpu (AP). BSP uses thread_init idle.
 * Returns 0 on success.
 */
int  thread_init_ap_idle(u32 u32Cpu);

/**
 * Pin thread by id to CPU slot (0=BSP). Default for all threads is 0.
 * AP runqueue: only threads with matching affinity are picked on that CPU.
 */
void thread_set_cpu(u32 u32ThrId, u32 u32Cpu);

/** Create kernel thread pinned to u32Cpu (0=BSP). Returns thr id or 0. */
u32  thread_create_on_cpu(struct gj_process *pProc, void (*pfn)(void *),
                          void *pArg, u32 u32Cpu);

/** Request remote CPU to schedule (IPI). No-op if same CPU or offline. */
void thread_resched_cpu(u32 u32Cpu);

/**
 * QoS class (Apple-shaped soft): see GJ_QOS_* (0..4). Affects pick order
 * among RUNNABLE threads on this CPU (higher rank first). Out-of-range
 * class clamps to GJ_QOS_NORMAL (soft clamp counted).
 */
void thread_set_qos(u32 u32ThrId, u32 u32Qos);
u32  thread_get_qos(u32 u32ThrId);

/**
 * Soft PI-shaped boost: temporary rank addend on thr, capped at
 * GJ_QOS_BOOST_CAP. Decays one tick each schedule leave. No door coupling.
 */
void thread_qos_boost_soft(u32 u32ThrId, u32 u32Ticks);

/**
 * Effective soft rank for thr (base QoS rank + residual boost, capped).
 * Returns 0 if thr unknown.
 */
u32  thread_qos_effective_rank(u32 u32ThrId);

/*
 * Greppable soft product counters (pick_next / QoS / kstack canary).
 * Prefix-stable line: "sched: soft stats ..."
 * Wrap OK; diagnostics only — never hard-gate scheduling decisions beyond
 * canary fail (which is a halt path when base/mid mismatch).
 */
struct gj_sched_soft_stats {
    u64 u64PickTotal;        /* non-null pick_next returns */
    u64 u64PickIdle;         /* idle thr selected */
    u64 u64PickInteractive;  /* base class picks */
    u64 u64PickNormal;
    u64 u64PickBackground;
    u64 u64PickUtility;
    u64 u64PickDriver;
    u64 u64PickAffSkip;      /* affinity mismatches walked */
    u64 u64PickEqualFair;    /* equal-rank wait-age wins */
    u64 u64PickSelf;         /* fell back to current */
    u64 u64QosSet;           /* thread_set_qos hits */
    u64 u64QosClamp;         /* class out of range → normal */
    u64 u64QosBoostSoft;     /* boost_soft applications */
    u64 u64QosBoostDecay;    /* boost ticks decayed on leave */
    u64 u64CanaryPlant;      /* base+mid plant ops */
    u64 u64CanaryCheck;      /* check invocations */
    u64 u64CanaryOk;         /* base+mid intact */
    u64 u64CanaryMidOk;      /* mid intact (subset of ok) */
    u64 u64CanaryFail;       /* base or mid mismatch (halt path) */
    u64 u64StackHwmMax;      /* max soft high-water bytes seen */
    u64 u64StackHwmSamples;  /* HWM soft samples taken */
};

/** Copy soft stats into *pOut (no-op if NULL). */
void thread_sched_soft_stats_get(struct gj_sched_soft_stats *pOut);

/**
 * Grep: sched: soft stats
 * Dump soft counters via kprintf. Returns u64PickTotal.
 */
u64  thread_sched_soft_stats_print(void);
