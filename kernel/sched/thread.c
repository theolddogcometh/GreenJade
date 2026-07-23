/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative threads + RR/QoS pick. See thread.h for dual-stack layout and
 * residual-#UD invariants (TSS.RSP0 dedicated IRQ stack; per-thr SYSCALL
 * USER_* save/restore across schedule).
 *
 * Soft product deepen (this unit only):
 *   - QoS classes 0..4 + capped soft boost (Apple §8 spirit)
 *   - pick_next soft stats + equal-rank wait-age fairness
 *   - kstack base+mid canary + poison HWM soft scan
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
static u8 g_aThrQos[GJ_MAX_THREADS]; /* GJ_QOS_* base class */
/* Soft PI residual boost ticks (capped); decayed on schedule leave. */
static u8 g_aThrBoost[GJ_MAX_THREADS];
/* Soft wait-age: last pick gen for equal-rank fairness. */
static u32 g_aThrLastPick[GJ_MAX_THREADS];
static u32 g_u32PickGen;

/* Soft preemption flag (BSP idle path; not per-CPU). */
static volatile int g_fYieldReq;

/* Soft product counters (pick / QoS / canary). */
static struct gj_sched_soft_stats g_soft;
static int g_fSoftStatsOnce; /* one-shot soft dump after warm picks */

static void thread_trampoline(void);

static u32
thr_index(struct gj_thread *pThr)
{
    if (pThr == NULL) {
        return GJ_MAX_THREADS;
    }
    if (pThr < &g_aThreads[0] || pThr >= &g_aThreads[GJ_MAX_THREADS]) {
        return GJ_MAX_THREADS;
    }
    return (u32)(pThr - &g_aThreads[0]);
}

/*
 * aKstack grows down from top. Plant base canary at low address + mid
 * soft canary so overflow stomps markers before adjacent thr fields.
 * Poison fill enables soft high-water measurement.
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
    u64 *pMid;

    if (pThr == NULL) {
        return;
    }
    /* Soft poison: unused depth stays GJ_THR_KSTACK_POISON for HWM scan. */
    memset(pThr->aKstack, (int)GJ_THR_KSTACK_POISON, GJ_THR_KSTACK_SIZE);
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    *pCan = GJ_THR_KSTACK_CANARY;
    pThr->u64KstackCanary = GJ_THR_KSTACK_CANARY;
    pMid = (u64 *)(void *)&pThr->aKstack[GJ_THR_KSTACK_MID];
    *pMid = GJ_THR_KSTACK_CANARY_MID;
    g_soft.u64CanaryPlant++;
}

/*
 * Soft high-water: bytes from top down to first non-poison (stack grows down).
 * Skips base canary region; mid canary may register as used if crossed.
 */
static u32
thr_kstack_hwm_soft(struct gj_thread *pThr)
{
    u8 *pBase;
    u8 *pTop;
    u8 *pScan;
    u32 cbUsed;

    if (pThr == NULL) {
        return 0;
    }
    pBase = pThr->aKstack;
    pTop = pBase + GJ_THR_KSTACK_SIZE;
    pScan = pTop;
    while (pScan > pBase + sizeof(u64)) {
        u32 u32Off;

        pScan--;
        u32Off = (u32)(pScan - pBase);
        /* Mid canary is planted non-poison — skip for HWM soft only. */
        if (u32Off >= GJ_THR_KSTACK_MID &&
            u32Off < GJ_THR_KSTACK_MID + (u32)sizeof(u64)) {
            continue;
        }
        if (*pScan != GJ_THR_KSTACK_POISON) {
            pScan++;
            break;
        }
    }
    cbUsed = (u32)(pTop - pScan);
    g_soft.u64StackHwmSamples++;
    if ((u64)cbUsed > g_soft.u64StackHwmMax) {
        g_soft.u64StackHwmMax = (u64)cbUsed;
    }
    return cbUsed;
}

static void
thread_check_kstack(struct gj_thread *pThr)
{
    u64 *pCan;
    u64 *pMid;

    if (pThr == NULL) {
        return;
    }
    /* Unplanted (0) skips — only before thr_plant_kstack_canary. */
    if (pThr->u64KstackCanary == 0) {
        return;
    }
    g_soft.u64CanaryCheck++;
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    pMid = (u64 *)(void *)&pThr->aKstack[GJ_THR_KSTACK_MID];
    if (*pCan != pThr->u64KstackCanary) {
        g_soft.u64CanaryFail++;
        kprintf("sched: KSTACK OVERFLOW thr=%u canary=0x%lx got=0x%lx — halt\n",
                pThr->u32Id, (unsigned long)pThr->u64KstackCanary,
                (unsigned long)*pCan);
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }
    if (*pMid != GJ_THR_KSTACK_CANARY_MID) {
        g_soft.u64CanaryFail++;
        kprintf("sched: KSTACK MID OVERFLOW thr=%u mid=0x%lx got=0x%lx — halt\n",
                pThr->u32Id, (unsigned long)GJ_THR_KSTACK_CANARY_MID,
                (unsigned long)*pMid);
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }
    g_soft.u64CanaryMidOk++;
    g_soft.u64CanaryOk++;
    (void)thr_kstack_hwm_soft(pThr);
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
    memset(g_aThrBoost, 0, sizeof(g_aThrBoost));
    memset(g_aThrLastPick, 0, sizeof(g_aThrLastPick));
    memset(&g_soft, 0, sizeof(g_soft));
    g_u32PickGen = 1;
    g_fSoftStatsOnce = 0;
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
    g_aThrQos[0] = GJ_QOS_NORMAL;
    g_apIdle[0] = g_pIdle;
    cpu_set_current_thread(g_pIdle);
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        g_aThreads[iThr].u32State = GJ_THR_UNUSED;
        g_aThrCpu[iThr] = 0;
        g_aThrQos[iThr] = GJ_QOS_NORMAL;
        g_aThrBoost[iThr] = 0;
    }
    for (iThr = 1; iThr < GJ_CPU_STATIC_MAX; iThr++) {
        g_apIdle[iThr] = NULL;
    }
    kprintf("sched: thread_init idle id=%u kstack=0x%lx\n", g_pIdle->u32Id,
            (unsigned long)g_pIdle->u64KstackTop);
    /* Soft product markers (greppable; prefix-stable). */
    kprintf("sched: soft qos classes=0..4 (norm/int/bg/util/drv) boost_cap=%u\n",
            GJ_QOS_BOOST_CAP);
    kprintf("sched: soft kstack canary base+mid hwm poison=0x%x mid_off=%u\n",
            (unsigned)GJ_THR_KSTACK_POISON, (unsigned)GJ_THR_KSTACK_MID);
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
    g_aThrQos[iThr] = GJ_QOS_NORMAL;
    g_aThrBoost[iThr] = 0;
    g_aThrLastPick[iThr] = 0;
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
    g_aThrQos[iThr] = GJ_QOS_NORMAL;
    g_aThrBoost[iThr] = 0;
    g_aThrLastPick[iThr] = 0;
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

/*
 * Base QoS → rank (higher first). Historical 0/1/2 order preserved:
 * interactive > normal > background. Soft deepen adds utility + driver.
 */
static u8
qos_rank(u8 u8Qos)
{
    /* DRIVER(4) > INTERACTIVE(1) > NORMAL(0) > UTILITY(3) > BACKGROUND(2) */
    if (u8Qos == GJ_QOS_DRIVER) {
        return 4;
    }
    if (u8Qos == GJ_QOS_INTERACTIVE) {
        return 3;
    }
    if (u8Qos == GJ_QOS_UTILITY) {
        return 1;
    }
    if (u8Qos == GJ_QOS_BACKGROUND) {
        return 0;
    }
    /* NORMAL and unknown → middle-high default */
    return 2;
}

static u8
qos_rank_eff(u32 u32Idx)
{
    u8 u8R;

    if (u32Idx >= GJ_MAX_THREADS) {
        return 0;
    }
    u8R = qos_rank(g_aThrQos[u32Idx]);
    /* Soft capped PI: residual boost lifts rank but never past driver. */
    if (g_aThrBoost[u32Idx] != 0) {
        u8 u8Cap = 4;

        if ((u16)u8R + (u16)g_aThrBoost[u32Idx] >= (u16)u8Cap) {
            u8R = u8Cap;
        } else {
            u8R = (u8)(u8R + g_aThrBoost[u32Idx]);
        }
    }
    return u8R;
}

static void
pick_note_class(u8 u8Qos)
{
    if (u8Qos == GJ_QOS_INTERACTIVE) {
        g_soft.u64PickInteractive++;
    } else if (u8Qos == GJ_QOS_BACKGROUND) {
        g_soft.u64PickBackground++;
    } else if (u8Qos == GJ_QOS_UTILITY) {
        g_soft.u64PickUtility++;
    } else if (u8Qos == GJ_QOS_DRIVER) {
        g_soft.u64PickDriver++;
    } else {
        g_soft.u64PickNormal++;
    }
}

static void
pick_soft_maybe_dump(void)
{
    /* One-shot warm soft dump after enough picks (greppable product depth). */
    if (g_fSoftStatsOnce == 0 && g_soft.u64PickTotal >= 64ull) {
        g_fSoftStatsOnce = 1;
        (void)thread_sched_soft_stats_print();
    }
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
    struct gj_thread *pCur;
    u32 u32BestIdx = 0;
    u8 u8BestRank = 0;
    u32 u32BestWait = 0;

    if (u32Cpu >= GJ_CPU_STATIC_MAX) {
        u32Cpu = 0;
    }
    u32Start = g_aRobin[u32Cpu];

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        u32 u32Idx = (u32Start + iThr) % GJ_MAX_THREADS;
        struct gj_thread *pThr = &g_aThreads[u32Idx];
        u8 u8R;
        u32 u32Wait;

        if (!thr_ok_on_cpu(u32Idx, u32Cpu)) {
            g_soft.u64PickAffSkip++;
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
        u8R = qos_rank_eff(u32Idx);
        /* Soft wait-age: gen delta since last pick (equal-rank fairness). */
        u32Wait = g_u32PickGen - g_aThrLastPick[u32Idx];
        if (pBest == NULL || u8R > u8BestRank ||
            (u8R == u8BestRank && u32Wait > u32BestWait)) {
            if (pBest != NULL && u8R == u8BestRank && u32Wait > u32BestWait) {
                g_soft.u64PickEqualFair++;
            }
            pBest = pThr;
            u32BestIdx = u32Idx;
            u8BestRank = u8R;
            u32BestWait = u32Wait;
        }
    }
    if (pBest != NULL) {
        g_aRobin[u32Cpu] = (u32BestIdx + 1) % GJ_MAX_THREADS;
        g_aThrLastPick[u32BestIdx] = g_u32PickGen;
        g_u32PickGen++;
        g_soft.u64PickTotal++;
        pick_note_class(g_aThrQos[u32BestIdx]);
        pick_soft_maybe_dump();
        return pBest;
    }
    pIdle = cpu_idle(u32Cpu);
    if (pIdle != NULL && pIdle->u32State != GJ_THR_EXITED) {
        u32 u32IdleIdx;

        if (pIdle->u32State == GJ_THR_BLOCKED ||
            pIdle->u32State == GJ_THR_UNUSED) {
            pIdle->u32State = GJ_THR_RUNNABLE;
        }
        u32IdleIdx = thr_index(pIdle);
        if (u32IdleIdx < GJ_MAX_THREADS) {
            g_aThrLastPick[u32IdleIdx] = g_u32PickGen;
        }
        g_u32PickGen++;
        g_soft.u64PickTotal++;
        g_soft.u64PickIdle++;
        pick_soft_maybe_dump();
        return pIdle;
    }
    pCur = thread_current();
    g_soft.u64PickTotal++;
    g_soft.u64PickSelf++;
    pick_soft_maybe_dump();
    return pCur;
}

void
thread_set_qos(u32 u32ThrId, u32 u32Qos)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            if (u32Qos <= GJ_QOS_CLASS_MAX) {
                g_aThrQos[iThr] = (u8)u32Qos;
            } else {
                g_aThrQos[iThr] = GJ_QOS_NORMAL;
                g_soft.u64QosClamp++;
            }
            g_soft.u64QosSet++;
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
    return GJ_QOS_NORMAL;
}

void
thread_qos_boost_soft(u32 u32ThrId, u32 u32Ticks)
{
    u32 iThr;
    u8 u8Add;

    if (u32Ticks == 0) {
        return;
    }
    u8Add = (u8)(u32Ticks > GJ_QOS_BOOST_CAP ? GJ_QOS_BOOST_CAP : u32Ticks);
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            u16 u16Sum = (u16)g_aThrBoost[iThr] + (u16)u8Add;

            if (u16Sum > GJ_QOS_BOOST_CAP) {
                g_aThrBoost[iThr] = (u8)GJ_QOS_BOOST_CAP;
            } else {
                g_aThrBoost[iThr] = (u8)u16Sum;
            }
            g_soft.u64QosBoostSoft++;
            return;
        }
    }
}

u32
thread_qos_effective_rank(u32 u32ThrId)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            return (u32)qos_rank_eff(iThr);
        }
    }
    return 0;
}

void
thread_sched_soft_stats_get(struct gj_sched_soft_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    *pOut = g_soft;
}

u64
thread_sched_soft_stats_print(void)
{
    /* Grep: sched: soft stats */
    kprintf("sched: soft stats pick=%lu idle=%lu int=%lu norm=%lu bg=%lu "
            "util=%lu drv=%lu aff_skip=%lu eq_fair=%lu self=%lu "
            "qos_set=%lu qos_clamp=%lu boost=%lu decay=%lu "
            "can_plant=%lu can_chk=%lu can_ok=%lu can_mid=%lu can_fail=%lu "
            "hwm_max=%lu hwm_n=%lu\n",
            (unsigned long)g_soft.u64PickTotal,
            (unsigned long)g_soft.u64PickIdle,
            (unsigned long)g_soft.u64PickInteractive,
            (unsigned long)g_soft.u64PickNormal,
            (unsigned long)g_soft.u64PickBackground,
            (unsigned long)g_soft.u64PickUtility,
            (unsigned long)g_soft.u64PickDriver,
            (unsigned long)g_soft.u64PickAffSkip,
            (unsigned long)g_soft.u64PickEqualFair,
            (unsigned long)g_soft.u64PickSelf,
            (unsigned long)g_soft.u64QosSet,
            (unsigned long)g_soft.u64QosClamp,
            (unsigned long)g_soft.u64QosBoostSoft,
            (unsigned long)g_soft.u64QosBoostDecay,
            (unsigned long)g_soft.u64CanaryPlant,
            (unsigned long)g_soft.u64CanaryCheck,
            (unsigned long)g_soft.u64CanaryOk,
            (unsigned long)g_soft.u64CanaryMidOk,
            (unsigned long)g_soft.u64CanaryFail,
            (unsigned long)g_soft.u64StackHwmMax,
            (unsigned long)g_soft.u64StackHwmSamples);
    return g_soft.u64PickTotal;
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
        u32 u32CurIdx;

        thread_check_kstack(pCur);
        if (pCur->u32State == GJ_THR_RUNNING) {
            pCur->u32State = GJ_THR_RUNNABLE;
        }
        /* Soft PI decay: one boost tick per leave (capped residual). */
        u32CurIdx = thr_index(pCur);
        if (u32CurIdx < GJ_MAX_THREADS && g_aThrBoost[u32CurIdx] != 0) {
            g_aThrBoost[u32CurIdx]--;
            g_soft.u64QosBoostDecay++;
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
