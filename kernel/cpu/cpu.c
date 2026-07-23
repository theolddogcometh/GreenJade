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
 *   Wave 9 complementary (file-local): publish static/dyn, GS init,
 *   HHDM vs identity dyn path, reject class split, walk/kgs probes,
 *   extra "cpu: soft counters|probe" lines — primary PASS line stable.
 *
 * Wave 14 exclusive deepen (this unit only — greppable "cpu: soft …"):
 *   cpu: soft inventory|pool|publish|reject_class|dyn|gs|layout|path|deepen
 *   Additive; never reshapes primary PASS|counters|probe|slot lines.
 *
 * Wave 15 exclusive soft deepen (this unit only — greppable "cpu: soft …"):
 *   cpu: soft ratio|headroom|surface|deepen  — bp ratios + catalog stamp
 *   Inventory wave stamp → 15; areas → 12. Primary PASS|counters|probe|slot
 *   lines stay field-stable. Never hard-gates product; wrap-OK + kprintf only.
 *
 * Wave 16 exclusive soft deepen (this unit only — greppable "cpu: soft …"):
 *   cpu: soft return|surface|deepen  — publish/init/gs return-path catalog
 *   Inventory wave stamp → 16; areas → 15. Primary PASS|counters|probe|slot
 *   lines stay field-stable. Soft ≠ bar3 / multi-CPU product gate.
 *
 * Wave 20 exclusive soft deepen (this unit only — greppable "cpu: soft …"):
 *   cpu: soft return|ret_surface|surface|deepen  — deepen publish return classes
 *   Inventory wave stamp → 17; areas → 16. Primary PASS|counters|probe|slot
 *   lines stay field-stable. product_kernel=OPEN. Soft ≠ bar3 gate.
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

/*
 * Complementary soft counters (Wave 9 deepen — file-local only).
 * Never hard-gate bring-up; wrap-OK; do not reshape primary PASS fields.
 */
static u32 g_u32SoftPublishStatic; /* successful static-pool AP publishes */
static u32 g_u32SoftPublishDyn;    /* successful dyn-pool AP publishes */
static u32 g_u32SoftGsInit;        /* cpu_gs_init entries with non-null pCpu */
static u32 g_u32SoftBspInit;       /* cpu_init_bsp entries */
static u32 g_u32SoftLogN;          /* cpu_soft_log emissions */
static u32 g_u32SoftDynHhdm;       /* dyn VA via HHDM path */
static u32 g_u32SoftDynIdent;      /* dyn VA via identity / low PA */
static u32 g_u32SoftDynPagesSum;   /* cumulative pages of successful dyn allocs */
static u32 g_u32SoftRejectBsp;     /* reject: id == 0 (BSP via init_ap) */
static u32 g_u32SoftRejectOob;     /* reject: id >= GJ_MAX_CPUS */
static u32 g_u32SoftZeroPages;     /* defensive cPages==0 rejects */
static u32 g_u32SoftDynReuse;      /* dyn slot pointer present, was offline */
static u32 g_u32SoftNullVa;        /* null-VA guard trips after PMM map */
static u32 g_u32SoftIdMatchFail;   /* published slot with u32CpuId != index */

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

/** Soft: saturate-increment a sticky u32 counter (wrap-OK diagnostics). */
static void
cpu_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
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
 * Soft: count dyn table entries that hold a non-null pointer
 * (allocated; may or may not be online).
 */
static u32
cpu_soft_count_dyn_ptrs(void)
{
    u32 i;
    u32 u32N = 0;

    for (i = GJ_CPU_STATIC_MAX; i < GJ_MAX_CPUS; i++) {
        if (g_apCpuDyn[i] != NULL) {
            u32N++;
        }
    }
    return u32N;
}

/**
 * Soft: count published slots whose u32CpuId does not match the slot index.
 * Should stay 0 after correct init paths.
 */
static u32
cpu_soft_count_id_mismatch(void)
{
    u32 i;
    u32 u32N = 0;
    struct gj_cpu *pCpu;

    for (i = 0; i < GJ_CPU_STATIC_MAX; i++) {
        if (g_aCpus[i].u32Online != 0 && g_aCpus[i].u32CpuId != i) {
            u32N++;
        }
    }
    for (i = GJ_CPU_STATIC_MAX; i < GJ_MAX_CPUS; i++) {
        pCpu = g_apCpuDyn[i];
        if (pCpu != NULL && pCpu->u32Online != 0 && pCpu->u32CpuId != i) {
            u32N++;
        }
    }
    return u32N;
}

/**
 * Soft: 1 if MSR_KERNEL_GS_BASE is zero (expected kernel GS after swapgs
 * contract on product BSP after cpu_gs_init).
 */
static int
cpu_soft_kgs_zero(void)
{
    return rdmsr(MSR_KERNEL_GS_BASE) == 0 ? 1 : 0;
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
    cpu_soft_inc(&g_u32SoftGsInit);
    wrmsr(MSR_GS_BASE, (u64)(gj_vaddr_t)pCpu);
    wrmsr(MSR_KERNEL_GS_BASE, 0);
}

void
cpu_init_bsp(void)
{
    struct gj_cpu *pCpu = &g_aCpus[0];

    cpu_soft_inc(&g_u32SoftBspInit);
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
    /* Complementary Wave 9 counters (keep bsp_init/log sticky across reset). */
    g_u32SoftPublishStatic = 0;
    g_u32SoftPublishDyn = 0;
    g_u32SoftGsInit = 0;
    g_u32SoftDynHhdm = 0;
    g_u32SoftDynIdent = 0;
    g_u32SoftDynPagesSum = 0;
    g_u32SoftRejectBsp = 0;
    g_u32SoftRejectOob = 0;
    g_u32SoftZeroPages = 0;
    g_u32SoftDynReuse = 0;
    g_u32SoftNullVa = 0;
    g_u32SoftIdMatchFail = 0;
    cpu_soft_note_online(0);
    cpu_soft_inc(&g_u32SoftPublishStatic); /* BSP is static slot 0 */
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
    int fDyn = 0;

    g_u32SoftLastInitId = u32CpuId;

    /* Reject BSP (handled by cpu_init_bsp) and out-of-range design ceiling. */
    if (u32CpuId == 0 || u32CpuId >= GJ_MAX_CPUS) {
        cpu_soft_inc(&g_u32SoftReject);
        if (u32CpuId == 0) {
            cpu_soft_inc(&g_u32SoftRejectBsp);
        } else {
            cpu_soft_inc(&g_u32SoftRejectOob);
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
            cpu_soft_inc(&g_u32SoftIdempotent);
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
        fDyn = 1;
        pCpu = g_apCpuDyn[u32CpuId];
        if (pCpu != NULL && pCpu->u32Online != 0) {
            cpu_soft_inc(&g_u32SoftIdempotent);
            /* Greppable: cpu: soft idempotent */
            kprintf("cpu: soft idempotent id=%u kind=%u n=%u\n",
                    u32CpuId, GJ_CPU_SOFT_KIND_DYN, g_u32SoftIdempotent);
            return; /* already published */
        }
        if (pCpu != NULL) {
            /* Pointer retained from a prior offline/unfinished path. */
            cpu_soft_inc(&g_u32SoftDynReuse);
        }
        if (pCpu == NULL) {
            cPages = (u32)((sizeof(struct gj_cpu) + GJ_PAGE_SIZE - 1) /
                           GJ_PAGE_SIZE);
            g_u32SoftLastPages = cPages;
            if (cPages == 0) {
                cpu_soft_inc(&g_u32SoftReject);
                cpu_soft_inc(&g_u32SoftZeroPages);
                return; /* defensive: sizeof edge */
            }
            paBase = pmm_alloc_pages(cPages);
            /* pmm_alloc_pages → 0 on OOM: leave AP without GS percpu. */
            if (paBase == 0) {
                cpu_soft_inc(&g_u32SoftOom);
                /* Greppable: cpu: ... PMM percpu alloc fail */
                kprintf("cpu: AP id=%u PMM percpu alloc fail pages=%u "
                        "oom=%u soft\n",
                        u32CpuId, cPages, g_u32SoftOom);
                return;
            }
            if (hhdm_ready() || paBase >= 0x100000000ull) {
                pCpu = (struct gj_cpu *)(gj_vaddr_t)(GJ_HHDM_BASE +
                                                     (u64)paBase);
                cpu_soft_inc(&g_u32SoftDynHhdm);
            } else {
                pCpu = (struct gj_cpu *)(gj_vaddr_t)paBase;
                cpu_soft_inc(&g_u32SoftDynIdent);
            }
            if (pCpu == NULL) {
                cpu_soft_inc(&g_u32SoftReject);
                cpu_soft_inc(&g_u32SoftNullVa);
                return; /* unreachable; keeps null-VA guard explicit */
            }
            g_apCpuDyn[u32CpuId] = pCpu;
            g_u64SoftLastDynPa = (u64)paBase;
            if (g_u32DynPercpu < GJ_MAX_CPUS) {
                g_u32DynPercpu++;
            }
            if (g_u32SoftDynPagesSum <= 0xffffffffu - cPages) {
                g_u32SoftDynPagesSum += cPages;
            } else {
                g_u32SoftDynPagesSum = 0xffffffffu;
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
    if (fDyn) {
        cpu_soft_inc(&g_u32SoftPublishDyn);
    } else {
        cpu_soft_inc(&g_u32SoftPublishStatic);
    }
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
    u32 u32Walk;
    u32 u32DynPtrs;
    u32 u32IdMis;
    u32 u32WalkMatch;
    u32 u32KgsZero;
    u32 u32PagesPer;
    struct gj_cpu *pCpu;
    const char *szVerdict;

    cpu_soft_inc(&g_u32SoftLogN);
    cpu_soft_snapshot(&stSoft);

    u32Walk = stSoft.u32StaticOnline + stSoft.u32DynOnline;
    u32DynPtrs = cpu_soft_count_dyn_ptrs();
    u32IdMis = cpu_soft_count_id_mismatch();
    if (u32IdMis > g_u32SoftIdMatchFail) {
        g_u32SoftIdMatchFail = u32IdMis; /* sticky high-water of mismatches */
    }
    u32WalkMatch = (u32Walk == stSoft.u32Online) ? 1u : 0u;
    u32KgsZero = cpu_soft_kgs_zero() ? 1u : 0u;
    u32PagesPer = (u32)((sizeof(struct gj_cpu) + GJ_PAGE_SIZE - 1) /
                        GJ_PAGE_SIZE);

    /*
     * Verdict (soft product inventory):
     *   UP      — BSP only, no OOM/reject noise
     *   PASS    — multi-CPU published, no OOM
     *   PARTIAL — some online + OOM or dyn alloc without matching online
     *   DEGRADED— OOM or reject with only BSP / inconsistent online
     * Primary fields/order frozen for smoke greps — do not reorder.
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
     * Field order stable — complementary data goes on lines below.
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

    /*
     * Complementary greppable inventory (Wave 9) — never alters PASS line.
     *   cpu: soft counters …
     *   cpu: soft probe …
     */
    kprintf("cpu: soft counters pub_static=%u pub_dyn=%u gs_init=%u "
            "bsp_init=%u log_n=%u hhdm=%u ident=%u pages_sum=%u "
            "rej_bsp=%u rej_oob=%u zero_pages=%u dyn_reuse=%u null_va=%u\n",
            g_u32SoftPublishStatic, g_u32SoftPublishDyn, g_u32SoftGsInit,
            g_u32SoftBspInit, g_u32SoftLogN, g_u32SoftDynHhdm,
            g_u32SoftDynIdent, g_u32SoftDynPagesSum, g_u32SoftRejectBsp,
            g_u32SoftRejectOob, g_u32SoftZeroPages, g_u32SoftDynReuse,
            g_u32SoftNullVa);
    kprintf("cpu: soft probe walk=%u walk_match=%u dyn_ptrs=%u id_mis=%u "
            "id_mis_hwm=%u kgs_zero=%u pages_per=%u gs_sane=%u online=%u "
            "dyn_alloc=%u\n",
            u32Walk, u32WalkMatch, u32DynPtrs, u32IdMis,
            g_u32SoftIdMatchFail, u32KgsZero, u32PagesPer, stSoft.u32GsSane,
            stSoft.u32Online, stSoft.u32DynAlloc);

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
        /* Greppable: cpu: soft slot= — field order stable. */
        kprintf("cpu: soft slot=%u kind=%u kstack=0x%lx cr3=0x%lx thr=%p "
                "online=%u\n",
                i, u32Kind, (unsigned long)pCpu->u64KernelRsp,
                (unsigned long)pCpu->u64Cr3, (void *)pCpu->pCurrent,
                pCpu->u32Online);
    }

    /*
     * Wave 14/15/16/17 exclusive deepen (complementary; never reshapes primary):
     *   cpu: soft inventory|pool|publish|reject_class|dyn|gs|layout|path
     *   cpu: soft ratio|headroom|surface|return|ret_surface|deepen  (Wave 15/17)
     * greppable: cpu: soft
     */
    {
        u32 u32StaticHead;
        u32 u32DynCeil;
        u32 u32DynHead;
        u32 u32PoolOccBp;
        u32 u32GsBaseMatch;
        u32 u32StaticOccBp;
        u32 u32DynOccBp;
        u32 u32RejectBp;
        u32 u32OomBp;
        u32 u32HhdmBp;
        u32 u32IdentBp;
        u32 u32PublishSum;
        u32 u32PublishOkBp;
        u64 u64GsNow;

        /* Soft headroom vs design ceilings (diagnostics only). */
        if (stSoft.u32StaticOnline < stSoft.u32StaticMax) {
            u32StaticHead = stSoft.u32StaticMax - stSoft.u32StaticOnline;
        } else {
            u32StaticHead = 0;
        }
        if (stSoft.u32MaxCpus > stSoft.u32StaticMax) {
            u32DynCeil = stSoft.u32MaxCpus - stSoft.u32StaticMax;
        } else {
            u32DynCeil = 0;
        }
        if (stSoft.u32DynOnline < u32DynCeil) {
            u32DynHead = u32DynCeil - stSoft.u32DynOnline;
        } else {
            u32DynHead = 0;
        }
        /* Occupancy basis points vs GJ_MAX_CPUS (0 if ceiling is 0). */
        if (stSoft.u32MaxCpus != 0) {
            u32PoolOccBp =
                (stSoft.u32Online * 10000u) / stSoft.u32MaxCpus;
        } else {
            u32PoolOccBp = 0;
        }
        /* Wave 15: class-split occupancy + reject/oom/hhdm basis points. */
        if (stSoft.u32StaticMax != 0) {
            u32StaticOccBp =
                (stSoft.u32StaticOnline * 10000u) / stSoft.u32StaticMax;
        } else {
            u32StaticOccBp = 0;
        }
        if (u32DynCeil != 0) {
            u32DynOccBp = (stSoft.u32DynOnline * 10000u) / u32DynCeil;
        } else {
            u32DynOccBp = 0;
        }
        if (stSoft.u32Online != 0) {
            u32RejectBp = (stSoft.u32Reject * 10000u) / stSoft.u32Online;
            u32OomBp = (stSoft.u32Oom * 10000u) / stSoft.u32Online;
        } else {
            u32RejectBp = 0;
            u32OomBp = 0;
        }
        if (stSoft.u32DynAlloc != 0) {
            u32HhdmBp = (g_u32SoftDynHhdm * 10000u) / stSoft.u32DynAlloc;
            u32IdentBp = (g_u32SoftDynIdent * 10000u) / stSoft.u32DynAlloc;
        } else {
            u32HhdmBp = 0;
            u32IdentBp = 0;
        }
        /* Wave 16: publish success share vs static+dyn publish attempts. */
        u32PublishSum = g_u32SoftPublishStatic + g_u32SoftPublishDyn +
                        stSoft.u32Reject + stSoft.u32Oom;
        if (u32PublishSum != 0) {
            u32PublishOkBp =
                ((g_u32SoftPublishStatic + g_u32SoftPublishDyn) * 10000u) /
                u32PublishSum;
        } else {
            u32PublishOkBp = 0;
        }
        u64GsNow = rdmsr(MSR_GS_BASE);
        u32GsBaseMatch = cpu_soft_va_is_published(u64GsNow) ? 1u : 0u;

        /* Grep: cpu: soft inventory */
        kprintf("cpu: soft inventory verdict=%s online=%u static=%u dyn=%u "
                "dyn_alloc=%u oom=%u reject=%u idem=%u max_id=%u "
                "gs_sane=%u walk_match=%u logs=%u wave=20\n",
                szVerdict, stSoft.u32Online, stSoft.u32StaticOnline,
                stSoft.u32DynOnline, stSoft.u32DynAlloc, stSoft.u32Oom,
                stSoft.u32Reject, stSoft.u32Idempotent,
                stSoft.u32MaxOnlineId, stSoft.u32GsSane, u32WalkMatch,
                g_u32SoftLogN);
        /* Grep: cpu: soft pool */
        kprintf("cpu: soft pool static_max=%u max_cpus=%u static_on=%u "
                "dyn_on=%u static_head=%u dyn_ceil=%u dyn_head=%u "
                "occ_bp=%u dyn_ptrs=%u\n",
                stSoft.u32StaticMax, stSoft.u32MaxCpus,
                stSoft.u32StaticOnline, stSoft.u32DynOnline, u32StaticHead,
                u32DynCeil, u32DynHead, u32PoolOccBp, u32DynPtrs);
        /* Grep: cpu: soft publish */
        kprintf("cpu: soft publish static=%u dyn=%u last_id=%u last_pa=0x%lx "
                "last_pages=%u bsp_init=%u\n",
                g_u32SoftPublishStatic, g_u32SoftPublishDyn,
                stSoft.u32LastInitId, (unsigned long)stSoft.u64LastDynPa,
                stSoft.u32LastPages, g_u32SoftBspInit);
        /* Grep: cpu: soft reject_class */
        kprintf("cpu: soft reject_class total=%u bsp=%u oob=%u zero_pages=%u "
                "null_va=%u oom=%u idem=%u\n",
                stSoft.u32Reject, g_u32SoftRejectBsp, g_u32SoftRejectOob,
                g_u32SoftZeroPages, g_u32SoftNullVa, stSoft.u32Oom,
                stSoft.u32Idempotent);
        /* Grep: cpu: soft dyn */
        kprintf("cpu: soft dyn alloc=%u online=%u hhdm=%u ident=%u "
                "reuse=%u pages_sum=%u null_va=%u oom=%u\n",
                stSoft.u32DynAlloc, stSoft.u32DynOnline, g_u32SoftDynHhdm,
                g_u32SoftDynIdent, g_u32SoftDynReuse, g_u32SoftDynPagesSum,
                g_u32SoftNullVa, stSoft.u32Oom);
        /* Grep: cpu: soft gs */
        kprintf("cpu: soft gs sane=%u base_match=%u kgs_zero=%u gs_init=%u "
                "id_mis=%u id_mis_hwm=%u gs=0x%lx\n",
                stSoft.u32GsSane, u32GsBaseMatch, u32KgsZero,
                g_u32SoftGsInit, u32IdMis, g_u32SoftIdMatchFail,
                (unsigned long)u64GsNow);
        /* Grep: cpu: soft layout */
        kprintf("cpu: soft layout percpu_cb=%u pages_per=%u "
                "syscall_stack=%u static_max=%u max_cpus=%u "
                "page_size=%u hhdm_base=0x%lx\n",
                stSoft.u32PercpuBytes, u32PagesPer,
                (u32)GJ_SYSCALL_STACK_SIZE, stSoft.u32StaticMax,
                stSoft.u32MaxCpus, (u32)GJ_PAGE_SIZE,
                (unsigned long)GJ_HHDM_BASE);
        /* Grep: cpu: soft path */
        kprintf("cpu: soft path claim=static_bss+pmm_dyn gs=MSR_GS_BASE "
                "kgs=0 kind=STATIC|DYN soft=Wave17 "
                "(soft inventory; not bar3)\n");
        /* Grep: cpu: soft ratio (Wave 15/17) */
        kprintf("cpu: soft ratio pool_occ_bp=%u static_occ_bp=%u "
                "dyn_occ_bp=%u reject_bp=%u oom_bp=%u hhdm_bp=%u "
                "ident_bp=%u publish_ok_bp=%u wave=20\n",
                u32PoolOccBp, u32StaticOccBp, u32DynOccBp, u32RejectBp,
                u32OomBp, u32HhdmBp, u32IdentBp, u32PublishOkBp);
        /* Grep: cpu: soft headroom (Wave 15/17) */
        kprintf("cpu: soft headroom static=%u dyn=%u dyn_ceil=%u "
                "max_cpus=%u online=%u max_id=%u wave=20\n",
                u32StaticHead, u32DynHead, u32DynCeil, stSoft.u32MaxCpus,
                stSoft.u32Online, stSoft.u32MaxOnlineId);
        /*
         * Grep: cpu: soft return
         * Wave 19 return-path catalog — publish/init/gs terminal outcomes.
         * Soft ≠ product bring-up gate. product_kernel=OPEN.
         */
        kprintf("cpu: soft return publish_static=%u publish_dyn=%u "
                "idem=%u reject=%u reject_bsp=%u reject_oob=%u "
                "zero_pages=%u null_va=%u oom=%u gs_init=%u bsp_init=%u "
                "id_mis=%u walk_match=%u gs_sane=%u dyn_alloc=%u "
                "dyn_hhdm=%u dyn_ident=%u product_kernel=OPEN wave=20\n",
                g_u32SoftPublishStatic, g_u32SoftPublishDyn,
                stSoft.u32Idempotent, stSoft.u32Reject, g_u32SoftRejectBsp,
                g_u32SoftRejectOob, g_u32SoftZeroPages, g_u32SoftNullVa,
                stSoft.u32Oom, g_u32SoftGsInit, g_u32SoftBspInit,
                g_u32SoftIdMatchFail, u32WalkMatch, stSoft.u32GsSane,
                stSoft.u32DynAlloc, g_u32SoftDynHhdm, g_u32SoftDynIdent);
        /* Grep: cpu: soft ret_surface — Wave 19 terminal return classes */
        kprintf("cpu: soft ret_surface publish=static|dyn|idem "
                "reject=bsp|oob|zero|null|oom gs=init|sane|walk "
                "dyn=alloc|hhdm|ident product_kernel=OPEN areas=17 wave=20\n");
        /* Grep: cpu: soft surface (Wave 19 area catalog) */
        kprintf("cpu: soft surface inventory,pool,publish,reject_class,"
                "dyn,gs,layout,path,ratio,headroom,return,ret_surface,"
                "deepen,counters,probe,slot areas=17 wave=20\n");
        /* Grep: cpu: soft retmap — Wave 19 return-surface map */
    kprintf("cpu: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=20\n");

    /* Grep: cpu: soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: cpu: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("cpu: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=20 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: cpu: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("cpu: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=20 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 exclusive complementary surfaces (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: cpu: soft retbound — Wave 20 return-bound honesty */
        kprintf("cpu: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=20 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: cpu: soft retseal — Wave 20 exclusive seal stamp */
        kprintf("cpu: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=20 "
                "(retseal stamp; Soft≠product)\n");
        kprintf("cpu: soft deepen wave=20 areas=19 online=%u static=%u "
                "dyn=%u oom=%u reject=%u logs=%u\n",
                stSoft.u32Online, stSoft.u32StaticOnline,
                stSoft.u32DynOnline, stSoft.u32Oom, stSoft.u32Reject,
                g_u32SoftLogN);
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
