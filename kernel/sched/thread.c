/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative threads + RR/QoS pick. See thread.h for dual-stack layout and
 * residual-#UD invariants (TSS.RSP0 dedicated IRQ stack; per-thr SYSCALL
 * USER_* save/restore across schedule).
 */
#include <gj/apic.h>
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/door.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/net_eth.h>
#include <gj/process.h>
#include <gj/session_input.h>
#include <gj/smp.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/vmm.h>

extern void switch_context(u64 *pOldRsp, u64 u64NewRsp);

static struct gj_thread g_aThreads[GJ_MAX_THREADS];
static struct gj_thread *g_pIdle;
static struct gj_thread *g_apIdle[GJ_CPU_STATIC_MAX]; /* per-CPU idle (0 = BSP) */
static u32 g_u32NextId = 1;
/* External affinity/QoS — keep out of struct to preserve layout stability. */
static u8 g_aThrCpu[GJ_MAX_THREADS]; /* 0=BSP default; 0xFF=any */
static u8 g_aThrQos[GJ_MAX_THREADS]; /* 0=normal 1=interactive 2=background */

/* Soft preemption flag (BSP idle path; not per-CPU). */
static volatile int g_fYieldReq;

static void thread_trampoline(void);

/*
 * aKstack grows down from top. Plant canary at low address so overflow
 * stomps it before smashing adjacent thr fields / other stacks.
 */
static u64
thr_kstack_top(struct gj_thread *pThr)
{
    if (pThr == NULL) {
        return 0;
    }
    return ((u64)(gj_vaddr_t)&pThr->aKstack[GJ_THR_KSTACK_SIZE]) & ~0xfull;
}

static void
thr_plant_kstack_canary(struct gj_thread *pThr)
{
    u64 *pCan;

    if (pThr == NULL) {
        return;
    }
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    *pCan = GJ_THR_KSTACK_CANARY;
    pThr->u64KstackCanary = GJ_THR_KSTACK_CANARY;
}

static void
thread_check_kstack(struct gj_thread *pThr)
{
    u64 *pCan;

    if (pThr == NULL) {
        return;
    }
    /* Unplanted (0) skips — only before thr_plant_kstack_canary. */
    if (pThr->u64KstackCanary == 0) {
        return;
    }
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    if (*pCan != pThr->u64KstackCanary) {
        kprintf("sched: KSTACK OVERFLOW thr=%u canary=0x%lx got=0x%lx — halt\n",
                pThr->u32Id, (unsigned long)pThr->u64KstackCanary,
                (unsigned long)*pCan);
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }
}

/*
 * Initial aStack frame for switch_context → ret into trampoline.
 * Layout must match switch_context pop order (r15..rbp, then ret).
 */
static void
thr_build_initial_rsp(struct gj_thread *pThr)
{
    u64 *pSp;

    if (pThr == NULL) {
        return;
    }
    pSp = (u64 *)(void *)&pThr->aStack[GJ_THR_STACK_SIZE];
    pSp = (u64 *)((u64)pSp & ~0xfull);
    *(--pSp) = (u64)(gj_vaddr_t)thread_trampoline; /* ret addr */
    *(--pSp) = 0; /* rbp */
    *(--pSp) = 0; /* rbx */
    *(--pSp) = 0; /* r12 */
    *(--pSp) = 0; /* r13 */
    *(--pSp) = 0; /* r14 */
    *(--pSp) = 0; /* r15 */
    pThr->u64Rsp = (u64)(gj_vaddr_t)pSp;
}

static struct gj_thread *
thr_find_by_id(u32 u32Id)
{
    u32 iThr;

    if (u32Id == 0) {
        return NULL;
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32Id) {
            return &g_aThreads[iThr];
        }
    }
    return NULL;
}

static void
thread_trampoline(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL &&
        (pThr->u32Flags & (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) !=
            0) {
        int fUser32 = (pThr->u32Flags & GJ_THR_F_USER32_ENTRY) != 0;

        pThr->u32Flags &= ~(GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY);
        if (pThr->pProc != NULL && pThr->pProc->u64Cr3 != 0) {
            process_as_activate(pThr->pProc);
        }
        /* First ring-3 entry: SYSCALL kstack ready; RSP0 stays IRQ stack. */
        thread_install_kstack(pThr);
        kprintf("sched: thr %u enter user%s rip=0x%lx rsp=0x%lx\n",
                pThr->u32Id, fUser32 ? "32" : "",
                (unsigned long)pThr->u64UserRip,
                (unsigned long)pThr->u64UserRsp);
        if (fUser32) {
            cpu_enter_user32(pThr->u64UserRip, pThr->u64UserRsp);
        } else {
            cpu_enter_user(pThr->u64UserRip, pThr->u64UserRsp);
        }
        /* not reached on success */
    }
    if (pThr != NULL && pThr->pfnEntry != NULL) {
        pThr->pfnEntry(pThr->pArg);
    }
    thread_exit();
}

void
thread_install_kstack(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu = cpu_current();

    if (pCpu != NULL && pThr != NULL && pThr->u64KstackTop != 0) {
        /* SYSCALL path only (GS:0). Not TSS.RSP0. */
        pCpu->u64KernelRsp = pThr->u64KstackTop;
    }
    /*
     * Ring-3 IRQs must not land on thr SYSCALL stacks. Always reassert the
     * dedicated IRQ stack here — even if pThr is NULL or kstack unset.
     */
    tss_use_irq_rsp0();
}

void
thread_init(void)
{
    u32 iThr;

    memset(g_aThreads, 0, sizeof(g_aThreads));
    memset(g_aThrCpu, 0, sizeof(g_aThrCpu)); /* all BSP-affine by default */
    memset(g_aThrQos, 0, sizeof(g_aThrQos));
    g_fYieldReq = 0;
    /* Slot 0 = idle/bootstrap thread representing current execution */
    g_pIdle = &g_aThreads[0];
    g_pIdle->u32Id = g_u32NextId++;
    g_pIdle->u32State = GJ_THR_RUNNING;
    g_pIdle->pProc = NULL;
    g_pIdle->u64Cr3 = cpu_read_cr3();
    g_pIdle->u64Rsp = 0;
    g_pIdle->u64KstackTop = thr_kstack_top(g_pIdle);
    thr_plant_kstack_canary(g_pIdle);
    g_pIdle->u32SysUserValid = 0;
    g_aThrCpu[0] = 0;
    g_apIdle[0] = g_pIdle;
    cpu_set_current_thread(g_pIdle);
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        g_aThreads[iThr].u32State = GJ_THR_UNUSED;
        g_aThrCpu[iThr] = 0;
    }
    for (iThr = 1; iThr < GJ_CPU_STATIC_MAX; iThr++) {
        g_apIdle[iThr] = NULL;
    }
    kprintf("sched: thread_init idle id=%u kstack=0x%lx\n", g_pIdle->u32Id,
            (unsigned long)g_pIdle->u64KstackTop);
}

int
thread_init_ap_idle(u32 u32Cpu)
{
    u32 iThr;
    struct gj_thread *pThr;

    if (u32Cpu == 0 || u32Cpu >= GJ_CPU_STATIC_MAX) {
        return -1;
    }
    if (g_apIdle[u32Cpu] != NULL) {
        return 0;
    }
    pThr = NULL;
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32State == GJ_THR_UNUSED ||
            g_aThreads[iThr].u32State == GJ_THR_EXITED) {
            pThr = &g_aThreads[iThr];
            break;
        }
    }
    if (pThr == NULL) {
        return -1;
    }
    memset(pThr, 0, sizeof(*pThr));
    g_aThrCpu[iThr] = (u8)u32Cpu;
    g_aThrQos[iThr] = 0;
    pThr->u32Id = g_u32NextId++;
    pThr->u32State = GJ_THR_RUNNABLE;
    pThr->pProc = NULL;
    pThr->u64Cr3 = cpu_read_cr3();
    pThr->pfnEntry = NULL;
    pThr->u64KstackTop = thr_kstack_top(pThr);
    thr_plant_kstack_canary(pThr);
    thr_build_initial_rsp(pThr);
    g_apIdle[u32Cpu] = pThr;
    kprintf("sched: AP idle cpu=%u thr=%u\n", u32Cpu, pThr->u32Id);
    return 0;
}

struct gj_thread *
thread_current(void)
{
    return cpu_current_thread();
}

u32
thread_create(struct gj_process *pProc, void (*pfn)(void *), void *pArg)
{
    u32 iThr;
    struct gj_thread *pThr;

    /* pfn may be NULL for user-entry threads (GJ_THR_F_USER_ENTRY). */
    pThr = NULL;
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32State == GJ_THR_UNUSED ||
            g_aThreads[iThr].u32State == GJ_THR_EXITED) {
            pThr = &g_aThreads[iThr];
            break;
        }
    }
    if (pThr == NULL) {
        return 0;
    }
    memset(pThr, 0, sizeof(*pThr));
    g_aThrCpu[iThr] = 0; /* BSP until thread_set_cpu */
    g_aThrQos[iThr] = 0;
    pThr->u32Id = g_u32NextId++;
    pThr->u32State = GJ_THR_RUNNABLE;
    pThr->pProc = pProc;
    pThr->u64Cr3 = pProc ? 0 : cpu_read_cr3(); /* 0 = inherit from process later */
    pThr->pfnEntry = pfn;
    pThr->pArg = pArg;
    pThr->u64KstackTop = thr_kstack_top(pThr);
    thr_plant_kstack_canary(pThr);
    /* u32SysUserValid left 0 — no mid-syscall state until first SYSCALL */
    thr_build_initial_rsp(pThr);
    kprintf("sched: create thr %u kstack=0x%lx sz=%u\n", pThr->u32Id,
            (unsigned long)pThr->u64KstackTop, GJ_THR_KSTACK_SIZE);
    return pThr->u32Id;
}

u32
thread_create_user(struct gj_process *pProc, u64 u64Entry, u64 u64Stack)
{
    u32 u32Id;
    struct gj_thread *pThr;

    if (u64Entry == 0 || u64Stack == 0) {
        return 0;
    }
    u32Id = thread_create(pProc, NULL, NULL);
    if (u32Id == 0) {
        return 0;
    }
    pThr = thr_find_by_id(u32Id);
    if (pThr == NULL) {
        return 0;
    }
    pThr->u32Flags |= GJ_THR_F_USER_ENTRY;
    pThr->u64UserRip = u64Entry;
    pThr->u64UserRsp = u64Stack;
    pThr->pfnEntry = NULL;
    kprintf("sched: create user thr %u entry=0x%lx\n", u32Id,
            (unsigned long)u64Entry);
    return u32Id;
}

u32
thread_create_user32(struct gj_process *pProc, u64 u64Entry, u64 u64Stack)
{
    u32 u32Id;
    struct gj_thread *pThr;

    if (u64Entry == 0 || u64Stack == 0) {
        return 0;
    }
    u32Id = thread_create(pProc, NULL, NULL);
    if (u32Id == 0) {
        return 0;
    }
    pThr = thr_find_by_id(u32Id);
    if (pThr == NULL) {
        return 0;
    }
    pThr->u32Flags |= GJ_THR_F_USER32_ENTRY;
    pThr->u64UserRip = u64Entry;
    pThr->u64UserRsp = u64Stack;
    pThr->pfnEntry = NULL;
    kprintf("sched: create user32 thr %u entry=0x%lx\n", u32Id,
            (unsigned long)u64Entry);
    return u32Id;
}

u32
thread_exec_replace(struct gj_process *pProc, u64 u64Entry, u64 u64Stack)
{
    u32 iThr;
    u32 cUpdated = 0;

    if (pProc == NULL || u64Entry == 0 || u64Stack == 0) {
        return 0;
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        struct gj_thread *pThr = &g_aThreads[iThr];

        if (pThr->u32State == GJ_THR_UNUSED || pThr->u32State == GJ_THR_EXITED) {
            continue;
        }
        if (pThr->pProc != pProc) {
            continue;
        }
        if ((pThr->u32Flags & (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) ==
            0) {
            continue;
        }
        pThr->u64UserRip = u64Entry;
        pThr->u64UserRsp = u64Stack;
        cUpdated++;
    }
    if (cUpdated > 0) {
        kprintf("sched: exec_replace n=%u entry=0x%lx sp=0x%lx\n", cUpdated,
                (unsigned long)u64Entry, (unsigned long)u64Stack);
    }
    return cUpdated;
}

u32
thread_get_state(u32 u32Id)
{
    struct gj_thread *pThr = thr_find_by_id(u32Id);

    if (pThr == NULL) {
        return GJ_THR_UNUSED;
    }
    return pThr->u32State;
}

void
thread_block(void *pBlockObj, u32 u32Tag)
{
    struct gj_thread *pThr = thread_current();

    if (pThr == NULL) {
        return;
    }
    pThr->pBlockObj = pBlockObj;
    pThr->u32BlockTag = u32Tag;
    pThr->u32State = GJ_THR_BLOCKED;
}

u32
thread_wake(void *pBlockObj, u32 u32Tag, u32 u32Max)
{
    u32 iThr;
    u32 u32N = 0;

    for (iThr = 0; iThr < GJ_MAX_THREADS && u32N < u32Max; iThr++) {
        struct gj_thread *pThr = &g_aThreads[iThr];

        if (pThr->u32State != GJ_THR_BLOCKED) {
            continue;
        }
        if (pThr->pBlockObj != pBlockObj) {
            continue;
        }
        if (u32Tag != 0 && pThr->u32BlockTag != u32Tag) {
            continue;
        }
        pThr->pBlockObj = NULL;
        pThr->u32BlockTag = 0;
        pThr->u32State = GJ_THR_RUNNABLE;
        u32N++;
    }
    return u32N;
}

static int
thr_ok_on_cpu(u32 u32Idx, u32 u32Cpu)
{
    u8 u8Aff = g_aThrCpu[u32Idx];

    /* 0xFF = any CPU; else must match affinity (default 0 = BSP only). */
    if (u8Aff == 0xFFu) {
        return 1;
    }
    return u8Aff == (u8)u32Cpu;
}

static struct gj_thread *
cpu_idle(u32 u32Cpu)
{
    if (u32Cpu < GJ_CPU_STATIC_MAX && g_apIdle[u32Cpu] != NULL) {
        return g_apIdle[u32Cpu];
    }
    if (u32Cpu == 0) {
        return g_pIdle;
    }
    return NULL;
}

static u8
qos_rank(u8 u8Qos)
{
    /* interactive(1) > normal(0) > background(2) */
    if (u8Qos == 1) {
        return 2;
    }
    if (u8Qos == 2) {
        return 0;
    }
    return 1;
}

static struct gj_thread *
pick_next(void)
{
    static u32 g_aRobin[GJ_CPU_STATIC_MAX];
    u32 iThr;
    u32 u32Cpu = cpu_id();
    u32 u32Start;
    struct gj_thread *pIdle;
    struct gj_thread *pBest = NULL;
    u32 u32BestIdx = 0;
    u8 u8BestRank = 0;

    if (u32Cpu >= GJ_CPU_STATIC_MAX) {
        u32Cpu = 0;
    }
    u32Start = g_aRobin[u32Cpu];

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        u32 u32Idx = (u32Start + iThr) % GJ_MAX_THREADS;
        struct gj_thread *pThr = &g_aThreads[u32Idx];
        u8 u8R;

        if (!thr_ok_on_cpu(u32Idx, u32Cpu)) {
            continue;
        }
        /* Idle threads only when nothing else is runnable */
        if (pThr == cpu_idle(u32Cpu)) {
            continue;
        }
        /*
         * Only RUNNABLE thr are candidates. A RUNNING current is not
         * re-picked here (self-switch is a no-op in schedule).
         */
        if (pThr->u32State != GJ_THR_RUNNABLE) {
            continue;
        }
        u8R = qos_rank(g_aThrQos[u32Idx]);
        if (pBest == NULL || u8R > u8BestRank) {
            pBest = pThr;
            u32BestIdx = u32Idx;
            u8BestRank = u8R;
        }
    }
    if (pBest != NULL) {
        g_aRobin[u32Cpu] = (u32BestIdx + 1) % GJ_MAX_THREADS;
        return pBest;
    }
    pIdle = cpu_idle(u32Cpu);
    if (pIdle != NULL && pIdle->u32State != GJ_THR_EXITED) {
        if (pIdle->u32State == GJ_THR_BLOCKED ||
            pIdle->u32State == GJ_THR_UNUSED) {
            pIdle->u32State = GJ_THR_RUNNABLE;
        }
        return pIdle;
    }
    return thread_current();
}

void
thread_set_qos(u32 u32ThrId, u32 u32Qos)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            g_aThrQos[iThr] = (u8)(u32Qos <= 2u ? u32Qos : 0);
            return;
        }
    }
}

u32
thread_get_qos(u32 u32ThrId)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId) {
            return g_aThrQos[iThr];
        }
    }
    return 0;
}

void
thread_resched_cpu(u32 u32Cpu)
{
    if (u32Cpu == cpu_id()) {
        return;
    }
    apic_send_resched(u32Cpu);
}

void
thread_set_cpu(u32 u32ThrId, u32 u32Cpu)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            g_aThrCpu[iThr] = (u8)u32Cpu;
            return;
        }
    }
}

u32
thread_create_on_cpu(struct gj_process *pProc, void (*pfn)(void *), void *pArg,
                     u32 u32Cpu)
{
    u32 u32Id = thread_create(pProc, pfn, pArg);

    if (u32Id != 0) {
        thread_set_cpu(u32Id, u32Cpu);
    }
    return u32Id;
}

/*
 * Snapshot per-CPU GS USER_* onto the thr before switch. Without this,
 * thr B's SYSCALL overwrites GS and thr A's later sysretq uses B's return
 * target (wrong user RIP/RSP → faults / #UD-class fallout).
 */
static void
thread_save_sys_user(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu;

    if (pThr == NULL) {
        return;
    }
    pCpu = cpu_current();
    if (pCpu == NULL) {
        return;
    }
    pThr->u64SysUserRip = pCpu->u64UserRip;
    pThr->u64SysUserRsp = pCpu->u64UserRsp;
    pThr->u64SysUserRflags = pCpu->u64UserRflags;
    pThr->u32SysUserValid = 1;
}

static void
thread_restore_sys_user(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu;

    if (pThr == NULL || pThr->u32SysUserValid == 0) {
        return;
    }
    pCpu = cpu_current();
    if (pCpu == NULL) {
        return;
    }
    pCpu->u64UserRip = pThr->u64SysUserRip;
    pCpu->u64UserRsp = pThr->u64SysUserRsp;
    pCpu->u64UserRflags = pThr->u64SysUserRflags;
}

void
schedule(void)
{
    struct gj_thread *pCur;
    struct gj_thread *pNext;
    struct gj_thread *pNow;
    u64 u64OldRsp;
    u64 u64KerCr3;

    pCur = thread_current();
    pNext = pick_next();
    if (pNext == NULL) {
        return;
    }
    if (pCur == pNext) {
        if (pCur != NULL && pCur->u32State == GJ_THR_BLOCKED) {
            /* Spin briefly for a wake (cooperative; no IRQ-driven unblock). */
            while (pick_next() == pCur && pCur->u32State == GJ_THR_BLOCKED) {
                __asm__ volatile ("pause");
            }
            pNext = pick_next();
            if (pNext == pCur) {
                return;
            }
        } else {
            return;
        }
    }

    if (pCur != NULL) {
        thread_check_kstack(pCur);
        if (pCur->u32State == GJ_THR_RUNNING) {
            pCur->u32State = GJ_THR_RUNNABLE;
        }
        /* Preserve SYSCALL return target across thr switch (per-CPU otherwise). */
        thread_save_sys_user(pCur);
    }
    pNext->u32State = GJ_THR_RUNNING;

    /*
     * Mark next current *before* switch so trampoline / thread_current()
     * see the right thr — but do NOT install SYSCALL kstack yet.
     * cpu_set_current_thread deliberately leaves u64KernelRsp alone;
     * thread_install_kstack runs only after switch_context returns here.
     *
     * Switch under kernel CR3 so thr stacks (kernel BSS) are always mapped.
     * Keep TSS.RSP0 on the dedicated IRQ stack so a parked thr's aKstack is
     * never the ring-3 interrupt stack of another thr.
     */
    cpu_set_current_thread(pNext);
    tss_use_irq_rsp0();
    u64KerCr3 = vmm_kernel_cr3() ? vmm_kernel_cr3() : cpu_read_cr3();
    cpu_load_cr3(u64KerCr3);
    vmm_set_anon_cursor(NULL);

    if (pCur == NULL) {
        u64OldRsp = 0;
        switch_context(&u64OldRsp, pNext->u64Rsp);
    } else {
        switch_context(&pCur->u64Rsp, pNext->u64Rsp);
    }

    /* Resumed on this thr's stack (locals are this schedule() invocation). */
    pNow = thread_current();
    if (pNow == NULL) {
        pNow = pNext;
        cpu_set_current_thread(pNow);
    }
    thread_check_kstack(pNow);
    if (pNow->pProc != NULL && pNow->pProc->u64Cr3 != 0) {
        process_as_activate(pNow->pProc);
    } else if (pNow->u64Cr3 != 0) {
        cpu_load_cr3(pNow->u64Cr3);
        vmm_set_anon_cursor(NULL);
    } else {
        cpu_load_cr3(u64KerCr3);
        vmm_set_anon_cursor(NULL);
    }
    /* Restore this thr's SYSCALL return target, then its SYSCALL kstack. */
    thread_restore_sys_user(pNow);
    thread_install_kstack(pNow);
}

void
thread_yield(void)
{
    g_fYieldReq = 0;
    schedule();
}

void
thread_yield_request(void)
{
    g_fYieldReq = 1;
}

int
thread_yield_pending(void)
{
    int fPending = g_fYieldReq;

    if (fPending) {
        g_fYieldReq = 0;
    }
    return fPending;
}

void
thread_exit(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL) {
        pThr->u32State = GJ_THR_EXITED;
        pThr->pfnEntry = NULL;
        /* Defensive: do not restore stale USER_* if slot is reused later. */
        pThr->u32SysUserValid = 0;
        /* Drop cold-door roles so callers never CAS-hang on this thr. */
        door_on_thread_exit(pThr);
    }
    schedule();
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void
scheduler_run(void)
{
    kprintf("sched: run loop (idle HLT when idle)\n");
    for (;;) {
        net_eth_poll(); /* ARP/UDP-echo while idle */
        session_input_poll(); /* fan-in virtio-input for session */
        (void)thread_yield_pending(); /* clear soft-preempt flag */
        schedule();
        /* G-SCHED: idle with HLT when nothing runnable besides us */
        {
            struct gj_thread *pNext = pick_next();
            struct gj_thread *pIdle = cpu_idle(cpu_id());

            if (pNext == pIdle || pNext == thread_current()) {
                if (timer_ready()) {
                    __asm__ volatile ("hlt");
                } else {
                    __asm__ volatile ("pause");
                }
            } else {
                __asm__ volatile ("pause");
            }
        }
    }
}

void
scheduler_run_ap(void)
{
    u32 u32Cpu = cpu_id();
    struct gj_thread *pIdle = cpu_idle(u32Cpu);

    kprintf("sched: AP run loop cpu=%u idle=%u\n", u32Cpu,
            pIdle != NULL ? pIdle->u32Id : 0);
    if (pIdle != NULL) {
        pIdle->u32State = GJ_THR_RUNNING;
        cpu_set_current_thread(pIdle);
        /* SYSCALL stack for AP idle; RSP0 still dedicated IRQ (shared TSS). */
        thread_install_kstack(pIdle);
    }
    for (;;) {
        smp_ap_poll_work();
        schedule();
        {
            struct gj_thread *pNext = pick_next();

            if (pNext == pIdle || pNext == thread_current()) {
                __asm__ volatile ("hlt");
            } else {
                __asm__ volatile ("pause");
            }
        }
    }
}
