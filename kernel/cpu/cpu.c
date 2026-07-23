/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Percpu pool + GS_BASE. Layout of struct gj_cpu is an asm contract —
 * see cpu.h; do not reorder leading fields (sched / SYSCALL / swapgs).
 *
 * Soft deepen (boot telemetry only — not hot-path locks):
 *   Static vs dyn online counts, OOM/reject/idempotent counters,
 *   max online id, last dyn PA/pages, GS sanity, greppable cpu_soft_log.
 */
#include <gj/cpu.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

#define MSR_GS_BASE        0xC0000101u
#define MSR_KERNEL_GS_BASE 0xC0000102u

/*
 * Static BSS pool for BSP + low AP slots (keeps kernel image under user maps).
 * Slots >= GJ_CPU_STATIC_MAX grow via PMM into g_apCpuDyn[] (HHDM/identity VA).
 * g_apCpuDyn is sparse: only indices in [GJ_CPU_STATIC_MAX, GJ_MAX_CPUS) publish.
 */
static struct gj_cpu g_aCpus[GJ_CPU_STATIC_MAX];
/* Sparse; only indices >= GJ_CPU_STATIC_MAX are published. BSS starts NULL. */
static struct gj_cpu *g_apCpuDyn[GJ_MAX_CPUS];
static u32 g_u32NOnline = 1;
/* Successful PMM-backed percpu allocations (boot telemetry / soft inventory). */
static u32 g_u32DynPercpu;

/* Soft sticky counters (boot / bring-up observability). */
static u32 g_u32SoftOom;
static u32 g_u32SoftReject;
static u32 g_u32SoftIdempotent;
static u32 g_u32SoftMaxOnlineId;
static u32 g_u32SoftLastInitId;
static u32 g_u32SoftLastPages;
static u64 g_u64SoftLastDynPa;

static u64
rdmsr(u32 u32Msr)
{
    u32 u32Lo, u32Hi;
    __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
    return ((u64)u32Hi << 32) | u32Lo;
}

static void
wrmsr(u32 u32Msr, u64 u64Val)
{
    u32 u32Lo = (u32)u64Val;
    u32 u32Hi = (u32)(u64Val >> 32);
    __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
    (void)rdmsr;
}

/** Soft: mark slot published — bump max online id. */
static void
cpu_soft_note_online(u32 u32CpuId)
{
    if (u32CpuId > g_u32SoftMaxOnlineId) {
        g_u32SoftMaxOnlineId = u32CpuId;
    }
}

/** Soft: count online slots in static BSS pool. */
static u32
cpu_soft_count_static(void)
{
    u32 i;
    u32 u32N = 0;

    for (i = 0; i < GJ_CPU_STATIC_MAX; i++) {
        if (g_aCpus[i].u32Online != 0) {
            u32N++;
        }
    }
    return u32N;
}

/** Soft: count online PMM-dyn slots. */
static u32
cpu_soft_count_dyn(void)
{
    u32 i;
    u32 u32N = 0;
    struct gj_cpu *pCpu;

    for (i = GJ_CPU_STATIC_MAX; i < GJ_MAX_CPUS; i++) {
        pCpu = g_apCpuDyn[i];
        if (pCpu != NULL && pCpu->u32Online != 0) {
            u32N++;
        }
    }
    return u32N;
}

/**
 * Soft: true if va matches a published online percpu (static or dyn).
 * Used by GS sanity probe — walks pool, not hot path.
 */
static int
cpu_soft_va_is_published(u64 u64Va)
{
    u32 i;
    struct gj_cpu *pCpu;

    if (u64Va == 0) {
        return 0;
    }
    for (i = 0; i < GJ_CPU_STATIC_MAX; i++) {
        if (g_aCpus[i].u32Online != 0 &&
            (u64)(gj_vaddr_t)&g_aCpus[i] == u64Va) {
            return 1;
        }
    }
    for (i = GJ_CPU_STATIC_MAX; i < GJ_MAX_CPUS; i++) {
        pCpu = g_apCpuDyn[i];
        if (pCpu != NULL && pCpu->u32Online != 0 &&
            (u64)(gj_vaddr_t)pCpu == u64Va) {
            return 1;
        }
    }
    return 0;
}

void
cpu_gs_init(struct gj_cpu *pCpu)
{
    /*
     * Kernel: GS_BASE = percpu.
     * KERNEL_GS_BASE = 0 (user GS).
     * enter_user: swapgs → GS=0, KERNEL_GS=percpu; SYSCALL: swapgs → GS=percpu.
     */
    if (pCpu == NULL) {
        return;
    }
    wrmsr(MSR_GS_BASE, (u64)(gj_vaddr_t)pCpu);
    wrmsr(MSR_KERNEL_GS_BASE, 0);
}

void
cpu_init_bsp(void)
{
    struct gj_cpu *pCpu = &g_aCpus[0];

    memset(pCpu, 0, sizeof(*pCpu));
    pCpu->u32CpuId = 0;
    pCpu->u32Online = 1;
    pCpu->u64KernelRsp =
        (u64)(gj_vaddr_t)&pCpu->aSyscallStack[GJ_SYSCALL_STACK_SIZE];
    pCpu->u64Cr3 = cpu_read_cr3();
    pCpu->pCurrent = NULL;
    g_u32NOnline = 1;
    g_u32DynPercpu = 0;
    /* Soft sticky counters reset on BSP re-init (bring-up only). */
    g_u32SoftOom = 0;
    g_u32SoftReject = 0;
    g_u32SoftIdempotent = 0;
    g_u32SoftMaxOnlineId = 0;
    g_u32SoftLastInitId = 0;
    g_u32SoftLastPages = 0;
    g_u64SoftLastDynPa = 0;
    cpu_soft_note_online(0);
    cpu_gs_init(pCpu);
    /* Greppable: cpu: BSP ... */
    kprintf("cpu: BSP id=0 percpu=%p kstack=%lx cr3=%lx static_max=%u max=%u\n",
            (void *)pCpu, (unsigned long)pCpu->u64KernelRsp,
            (unsigned long)pCpu->u64Cr3, GJ_CPU_STATIC_MAX, GJ_MAX_CPUS);
    /* Greppable soft inventory after BSP publish. */
    cpu_soft_log();
}

void
cpu_init_ap(u32 u32CpuId)
{
    struct gj_cpu *pCpu;
    u32 cPages;
    gj_paddr_t paBase;

    g_u32SoftLastInitId = u32CpuId;

    /* Reject BSP (handled by cpu_init_bsp) and out-of-range design ceiling. */
    if (u32CpuId == 0 || u32CpuId >= GJ_MAX_CPUS) {
        if (g_u32SoftReject < 0xffffffffu) {
            g_u32SoftReject++;
        }
        /* Greppable: cpu: soft reject */
        kprintf("cpu: soft reject id=%u (bsp_or_oob) reject=%u\n",
                u32CpuId, g_u32SoftReject);
        return;
    }
    if (u32CpuId < GJ_CPU_STATIC_MAX) {
        pCpu = &g_aCpus[u32CpuId];
        /* Idempotent: never re-memset a live online percpu (pCurrent/GS). */
        if (pCpu->u32Online != 0) {
            if (g_u32SoftIdempotent < 0xffffffffu) {
                g_u32SoftIdempotent++;
            }
            /* Greppable: cpu: soft idempotent */
            kprintf("cpu: soft idempotent id=%u kind=%u n=%u\n",
                    u32CpuId, GJ_CPU_SOFT_KIND_STATIC, g_u32SoftIdempotent);
            return;
        }
    } else {
        /*
         * PMM-backed percpu growth past the static BSS pool.
         * Round sizeof(gj_cpu) up to whole pages; PMM frames are already
         * reachable via low identity map or HHDM (once hhdm_ready()).
         * Reuse a published dyn slot on re-entry; never install a null VA.
         */
        pCpu = g_apCpuDyn[u32CpuId];
        if (pCpu != NULL && pCpu->u32Online != 0) {
            if (g_u32SoftIdempotent < 0xffffffffu) {
                g_u32SoftIdempotent++;
            }
            /* Greppable: cpu: soft idempotent */
            kprintf("cpu: soft idempotent id=%u kind=%u n=%u\n",
                    u32CpuId, GJ_CPU_SOFT_KIND_DYN, g_u32SoftIdempotent);
            return; /* already published */
        }
        if (pCpu == NULL) {
            cPages = (u32)((sizeof(struct gj_cpu) + GJ_PAGE_SIZE - 1) /
                           GJ_PAGE_SIZE);
            g_u32SoftLastPages = cPages;
            if (cPages == 0) {
                if (g_u32SoftReject < 0xffffffffu) {
                    g_u32SoftReject++;
                }
                return; /* defensive: sizeof edge */
            }
            paBase = pmm_alloc_pages(cPages);
            /* pmm_alloc_pages → 0 on OOM: leave AP without GS percpu. */
            if (paBase == 0) {
                if (g_u32SoftOom < 0xffffffffu) {
                    g_u32SoftOom++;
                }
                /* Greppable: cpu: ... PMM percpu alloc fail */
                kprintf("cpu: AP id=%u PMM percpu alloc fail pages=%u "
                        "oom=%u soft\n",
                        u32CpuId, cPages, g_u32SoftOom);
                return;
            }
            if (hhdm_ready() || paBase >= 0x100000000ull) {
                pCpu = (struct gj_cpu *)(gj_vaddr_t)(GJ_HHDM_BASE +
                                                     (u64)paBase);
            } else {
                pCpu = (struct gj_cpu *)(gj_vaddr_t)paBase;
            }
            if (pCpu == NULL) {
                if (g_u32SoftReject < 0xffffffffu) {
                    g_u32SoftReject++;
                }
                return; /* unreachable; keeps null-VA guard explicit */
            }
            g_apCpuDyn[u32CpuId] = pCpu;
            g_u64SoftLastDynPa = (u64)paBase;
            if (g_u32DynPercpu < GJ_MAX_CPUS) {
                g_u32DynPercpu++;
            }
            /* Greppable: cpu: ... PMM percpu pa= */
            kprintf("cpu: AP id=%u PMM percpu pa=0x%lx pages=%u dyn=%u\n",
                    u32CpuId, (unsigned long)paBase, cPages, g_u32DynPercpu);
        }
    }
    /* pCpu non-null: static address or a published dyn slot. */
    memset(pCpu, 0, sizeof(*pCpu));
    pCpu->u32CpuId = u32CpuId;
    pCpu->u32Online = 1;
    pCpu->u64KernelRsp =
        (u64)(gj_vaddr_t)&pCpu->aSyscallStack[GJ_SYSCALL_STACK_SIZE];
    pCpu->u64Cr3 = cpu_read_cr3();
    pCpu->pCurrent = NULL;
    cpu_gs_init(pCpu);
    if (g_u32NOnline < GJ_MAX_CPUS) {
        g_u32NOnline++;
    }
    cpu_soft_note_online(u32CpuId);
    /* Greppable: cpu: AP id=... online= */
    kprintf("cpu: AP id=%u percpu=%p online=%u dyn=%u kind=%u soft\n",
            u32CpuId, (void *)pCpu, g_u32NOnline, g_u32DynPercpu,
            (u32CpuId < GJ_CPU_STATIC_MAX) ? GJ_CPU_SOFT_KIND_STATIC
                                           : GJ_CPU_SOFT_KIND_DYN);
}

u32
cpu_online_count(void)
{
    return g_u32NOnline;
}

struct gj_cpu *
cpu_for_id(u32 u32CpuId)
{
    struct gj_cpu *pCpu;

    if (u32CpuId >= GJ_MAX_CPUS) {
        return NULL;
    }
    if (u32CpuId < GJ_CPU_STATIC_MAX) {
        pCpu = &g_aCpus[u32CpuId];
        return (pCpu->u32Online != 0) ? pCpu : NULL;
    }
    pCpu = g_apCpuDyn[u32CpuId];
    if (pCpu == NULL || pCpu->u32Online == 0) {
        return NULL;
    }
    return pCpu;
}

int
cpu_slot_online(u32 u32CpuId)
{
    return cpu_for_id(u32CpuId) != NULL ? 1 : 0;
}

u32
cpu_dyn_percpu_count(void)
{
    return g_u32DynPercpu;
}

/* ---- Soft percpu pool observability API ---------------------------- */

u32
cpu_soft_kind(u32 u32CpuId)
{
    if (u32CpuId >= GJ_MAX_CPUS) {
        return GJ_CPU_SOFT_KIND_NONE;
    }
    if (u32CpuId < GJ_CPU_STATIC_MAX) {
        return (g_aCpus[u32CpuId].u32Online != 0) ? GJ_CPU_SOFT_KIND_STATIC
                                                  : GJ_CPU_SOFT_KIND_NONE;
    }
    if (g_apCpuDyn[u32CpuId] != NULL &&
        g_apCpuDyn[u32CpuId]->u32Online != 0) {
        return GJ_CPU_SOFT_KIND_DYN;
    }
    return GJ_CPU_SOFT_KIND_NONE;
}

u32
cpu_soft_static_online(void)
{
    return cpu_soft_count_static();
}

u32
cpu_soft_dyn_online(void)
{
    return cpu_soft_count_dyn();
}

u32
cpu_soft_oom_count(void)
{
    return g_u32SoftOom;
}

u32
cpu_soft_max_online_id(void)
{
    return g_u32SoftMaxOnlineId;
}

u64
cpu_soft_kstack_top(u32 u32CpuId)
{
    struct gj_cpu *pCpu = cpu_for_id(u32CpuId);

    if (pCpu == NULL) {
        return 0;
    }
    return pCpu->u64KernelRsp;
}

u64
cpu_soft_cr3(u32 u32CpuId)
{
    struct gj_cpu *pCpu = cpu_for_id(u32CpuId);

    if (pCpu == NULL) {
        return 0;
    }
    return pCpu->u64Cr3;
}

int
cpu_soft_gs_sane(void)
{
    u64 u64Gs = rdmsr(MSR_GS_BASE);

    return cpu_soft_va_is_published(u64Gs);
}

void
cpu_soft_snapshot(struct gj_cpu_soft *pOut)
{
    if (pOut == NULL) {
        return;
    }
    pOut->u32Online = g_u32NOnline;
    pOut->u32StaticOnline = cpu_soft_count_static();
    pOut->u32DynOnline = cpu_soft_count_dyn();
    pOut->u32DynAlloc = g_u32DynPercpu;
    pOut->u32Oom = g_u32SoftOom;
    pOut->u32Reject = g_u32SoftReject;
    pOut->u32Idempotent = g_u32SoftIdempotent;
    pOut->u32MaxOnlineId = g_u32SoftMaxOnlineId;
    pOut->u32StaticMax = GJ_CPU_STATIC_MAX;
    pOut->u32MaxCpus = GJ_MAX_CPUS;
    pOut->u32LastInitId = g_u32SoftLastInitId;
    pOut->u32LastPages = g_u32SoftLastPages;
    pOut->u64LastDynPa = g_u64SoftLastDynPa;
    pOut->u32PercpuBytes = (u32)sizeof(struct gj_cpu);
    pOut->u32GsSane = cpu_soft_gs_sane() ? 1u : 0u;
}

void
cpu_soft_log(void)
{
    struct gj_cpu_soft stSoft;
    u32 i;
    u32 u32Kind;
    struct gj_cpu *pCpu;
    const char *szVerdict;

    cpu_soft_snapshot(&stSoft);

    /*
     * Verdict (soft product inventory):
     *   UP      — BSP only, no OOM/reject noise
     *   PASS    — multi-CPU published, no OOM
     *   PARTIAL — some online + OOM or dyn alloc without matching online
     *   DEGRADED— OOM or reject with only BSP / inconsistent online
     */
    if (stSoft.u32Oom != 0 && stSoft.u32Online <= 1u) {
        szVerdict = "DEGRADED";
    } else if (stSoft.u32Oom != 0 ||
               (stSoft.u32DynAlloc > stSoft.u32DynOnline)) {
        szVerdict = "PARTIAL";
    } else if (stSoft.u32Online > 1u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "UP";
    }

    /*
     * Greppable soft pool line (product / smoke inventory):
     *   cpu: soft PASS|UP|PARTIAL|DEGRADED online=… static=… dyn=… …
     */
    kprintf("cpu: soft %s online=%u static=%u dyn=%u dyn_alloc=%u oom=%u "
            "reject=%u idem=%u max_id=%u last_id=%u last_pa=0x%lx "
            "pages=%u percpu_cb=%u gs_sane=%u static_max=%u max_cpus=%u\n",
            szVerdict, stSoft.u32Online, stSoft.u32StaticOnline,
            stSoft.u32DynOnline, stSoft.u32DynAlloc, stSoft.u32Oom,
            stSoft.u32Reject, stSoft.u32Idempotent, stSoft.u32MaxOnlineId,
            stSoft.u32LastInitId, (unsigned long)stSoft.u64LastDynPa,
            stSoft.u32LastPages, stSoft.u32PercpuBytes, stSoft.u32GsSane,
            stSoft.u32StaticMax, stSoft.u32MaxCpus);

    /* Per-slot soft detail: published slots only (cap-bounded walk). */
    for (i = 0; i <= stSoft.u32MaxOnlineId && i < GJ_MAX_CPUS; i++) {
        u32Kind = cpu_soft_kind(i);
        if (u32Kind == GJ_CPU_SOFT_KIND_NONE) {
            continue;
        }
        pCpu = cpu_for_id(i);
        if (pCpu == NULL) {
            continue;
        }
        kprintf("cpu: soft slot=%u kind=%u kstack=0x%lx cr3=0x%lx thr=%p "
                "online=%u\n",
                i, u32Kind, (unsigned long)pCpu->u64KernelRsp,
                (unsigned long)pCpu->u64Cr3, (void *)pCpu->pCurrent,
                pCpu->u32Online);
    }
}

struct gj_cpu *
cpu_current(void)
{
    u64 u64Gs;

    __asm__ volatile ("mov %%gs:0, %0" : "=r"(u64Gs));
    /*
     * First field is u64KernelRsp — not a self pointer.
     * Use MSR_GS_BASE value instead.
     */
    u64Gs = rdmsr(MSR_GS_BASE);
    if (u64Gs == 0) {
        return &g_aCpus[0];
    }
    return (struct gj_cpu *)(gj_vaddr_t)u64Gs;
}

u32
cpu_id(void)
{
    return cpu_current()->u32CpuId;
}

void
cpu_set_current_thread(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu = cpu_current();

    pCpu->pCurrent = pThr;
    /*
     * Install SYSCALL stack top only when fully switched to this thr.
     * schedule() calls thread_install_kstack() after switch_context so we
     * never point KERNEL_RSP at pNext while still running on pCur's frame.
     * First entry (trampoline) still needs KERNEL_RSP before enter_user —
     * trampoline calls thread_install_kstack explicitly.
     */
    /* TSS.RSP0 stays the dedicated IRQ stack from gdt_init (not thr kstack). */
    (void)tss_get_rsp0;
    (void)pThr;
}

struct gj_thread *
cpu_current_thread(void)
{
    return cpu_current()->pCurrent;
}

u64
cpu_read_cr3(void)
{
    u64 u64Cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
    return u64Cr3;
}

void
cpu_load_cr3(u64 u64Cr3)
{
    struct gj_cpu *pCpu = cpu_current();

    if (u64Cr3 == 0) {
        return;
    }
    if (pCpu->u64Cr3 == u64Cr3) {
        return;
    }
    __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
    pCpu->u64Cr3 = u64Cr3;
}
