/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel threads + cooperative scheduler (block/wake/yield).
 *
 * Dual stacks per thread:
 *   aStack  — cooperative switch_context (schedule / trampoline)
 *   aKstack — per-thr SYSCALL stack so door_recv can block mid-call (G-PERS)
 *
 * Hard invariants (prior residual-#UD fix — do not regress):
 *   1) TSS.RSP0 always the dedicated IRQ stack (tss_use_irq_rsp0). Never
 *      pin RSP0 to a thr aKstack: a parked thr mid-syscall has frames there;
 *      another thr's ring-3 IRQ would smash them → garbage RIP / kernel #UD.
 *   2) Per-CPU GS USER_* (RIP/RSP/RFLAGS) is not per-thread. schedule()
 *      save/restore via u64SysUser* so yield mid-syscall sysretq's to the
 *      correct user context after another thr has run.
 *   3) Install thr SYSCALL kstack (u64KernelRsp) only after switch_context
 *      (or from the user-entry trampoline) — never while still on pCur.
 */
#pragma once

#include <gj/types.h>

struct gj_process;
struct gj_thread;

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
 */
#define GJ_THR_STACK_SIZE    (16u * 1024u)
#define GJ_THR_KSTACK_SIZE   (32u * 1024u)
#define GJ_MAX_THREADS       32
#define GJ_THR_KSTACK_CANARY 0xC4A1C4A1C4A1C4A1ull

#define GJ_THR_F_USER_ENTRY   (1u << 0) /* enter ring-3 (64-bit) on first run */
#define GJ_THR_F_USER32_ENTRY (1u << 1) /* enter ring-3 compat CS32 via iretq */

struct gj_thread {
    u32                 u32Id;
    u32                 u32State;
    struct gj_process  *pProc;
    u64                 u64Cr3;          /* address space; 0 = use cpu default */
    /* Saved kernel context for switch_context (aStack or mid-syscall aKstack) */
    u64                 u64Rsp;
    void               *pBlockObj;       /* futex key / cold request id owner */
    u32                 u32BlockTag;
    u32                 u32Flags;
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

void thread_init(void);

/**
 * Create runnable kernel thread. Returns thread id or 0 on failure.
 * pfn may be NULL for user-entry threads.
 */
u32 thread_create(struct gj_process *pProc, void (*pfn)(void *), void *pArg);

/**
 * Create a thread that enters ring-3 at u64Entry with stack u64Stack on first
 * schedule (G-PERS userspace personality).
 */
u32 thread_create_user(struct gj_process *pProc, u64 u64Entry, u64 u64Stack);

/**
 * Create a thread that enters 32-bit compat ring-3 (CS32) via iretq.
 */
u32 thread_create_user32(struct gj_process *pProc, u64 u64Entry, u64 u64Stack);

/**
 * After execve: rewrite all user-entry threads of pProc to (entry, stack).
 * Returns count of threads updated. Does not create new threads.
 */
u32 thread_exec_replace(struct gj_process *pProc, u64 u64Entry, u64 u64Stack);

/**
 * Query thread state (GJ_THR_*). Returns GJ_THR_UNUSED if id unknown.
 */
u32 thread_get_state(u32 u32Id);

void thread_yield(void);
/** Soft preemption: set flag; next schedule/idle path yields. */
void thread_yield_request(void);
/** Consume pending yield request; returns 1 if yield was requested. */
int  thread_yield_pending(void);
void thread_exit(void);

/** Block current thread (caller must schedule()). */
void thread_block(void *pBlockObj, u32 u32Tag);

/** Wake up to u32Max threads blocked on pBlockObj (u32Tag 0 = any tag). */
u32  thread_wake(void *pBlockObj, u32 u32Tag, u32 u32Max);

struct gj_thread *thread_current(void);

/**
 * Cooperative context switch. Saves/restores SYSCALL USER_* on the thr,
 * keeps TSS.RSP0 on the dedicated IRQ stack, installs next thr kstack only
 * after switch_context returns on that thr.
 */
void schedule(void);

/**
 * Point per-CPU SYSCALL stack (u64KernelRsp) at pThr->u64KstackTop and
 * reassert TSS.RSP0 = dedicated IRQ stack. Never uses thr kstack as RSP0.
 */
void thread_install_kstack(struct gj_thread *pThr);

/** Enter scheduler forever (idle + others). */
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
 * QoS class (Solaris/Apple-shaped): 0=normal, 1=interactive, 2=background.
 * Affects pick order among RUNNABLE threads on this CPU (higher first).
 */
void thread_set_qos(u32 u32ThrId, u32 u32Qos);
u32  thread_get_qos(u32 u32ThrId);
