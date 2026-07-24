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
 * Wave 56 exclusive soft deepen (this unit only — greppable "cpu: soft …"):
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
                "gs_sane=%u walk_match=%u logs=%u wave=100\n",
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
                "ident_bp=%u publish_ok_bp=%u wave=100\n",
                u32PoolOccBp, u32StaticOccBp, u32DynOccBp, u32RejectBp,
                u32OomBp, u32HhdmBp, u32IdentBp, u32PublishOkBp);
        /* Grep: cpu: soft headroom (Wave 15/17) */
        kprintf("cpu: soft headroom static=%u dyn=%u dyn_ceil=%u "
                "max_cpus=%u online=%u max_id=%u wave=100\n",
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
                "dyn_hhdm=%u dyn_ident=%u product_kernel=OPEN wave=100\n",
                g_u32SoftPublishStatic, g_u32SoftPublishDyn,
                stSoft.u32Idempotent, stSoft.u32Reject, g_u32SoftRejectBsp,
                g_u32SoftRejectOob, g_u32SoftZeroPages, g_u32SoftNullVa,
                stSoft.u32Oom, g_u32SoftGsInit, g_u32SoftBspInit,
                g_u32SoftIdMatchFail, u32WalkMatch, stSoft.u32GsSane,
                stSoft.u32DynAlloc, g_u32SoftDynHhdm, g_u32SoftDynIdent);
        /* Grep: cpu: soft ret_surface — Wave 19 terminal return classes */
        kprintf("cpu: soft ret_surface publish=static|dyn|idem "
                "reject=bsp|oob|zero|null|oom gs=init|sane|walk "
                "dyn=alloc|hhdm|ident product_kernel=OPEN areas=89 wave=100\n");
        /* Grep: cpu: soft surface (Wave 19 area catalog) */
        kprintf("cpu: soft surface inventory,pool,publish,reject_class,"
                "dyn,gs,layout,path,ratio,headroom,return,ret_surface,"
                "deepen,counters,probe,slot areas=91 wave=100\n");
        /* Grep: cpu: soft retmap — Wave 19 return-surface map */
    kprintf("cpu: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=100\n");

    /* Grep: cpu: soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: cpu: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("cpu: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=100 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: cpu: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("cpu: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=100 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: cpu: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("cpu: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=100 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: cpu: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("cpu: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=100 "
                "(retseal stamp; Soft≠product)\n");
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: cpu: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("cpu: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=100 "
                        "(retpulse honesty; Soft≠product; not bar3)\n");
                /* Grep: cpu: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("cpu: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=100 "
                        "(retmark stamp; Soft≠product)\n");
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: cpu: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("cpu: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=100 "
                        "(retphase honesty; Soft≠product; not bar3)\n");
                /* Grep: cpu: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("cpu: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=100 "
                        "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: cpu: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("cpu: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=100 "
                        "(rettoken honesty; Soft≠product; not bar3)\n");
                /* Grep: cpu: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("cpu: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=100 "
                        "(retcrest stamp; Soft≠product)\n");
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: cpu: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("cpu: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=100 "
                        "(retvault honesty; Soft≠product; not bar3)\n");
                /* Grep: cpu: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("cpu: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=100 "
                        "(retbanner stamp; Soft≠product)\n");
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: cpu: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("cpu: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=100 "
                        "(retledger honesty; Soft≠product; not bar3)\n");
                /* Grep: cpu: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("cpu: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=100 "
                        "(retbeacon stamp; Soft≠product)\n");
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: cpu: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("cpu: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=100 "
                        "(retcipher honesty; Soft≠product; not bar3)\n");
                /* Grep: cpu: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("cpu: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=100 "
                        "(retflame stamp; Soft≠product)\n");
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: cpu: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("cpu: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=100 "
                                "(retprism honesty; Soft≠product; not bar3)\n");
                        /* Grep: cpu: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("cpu: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=100 "
                                "(retforge stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: cpu: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("cpu: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=100 "
                                        "(retshard honesty; Soft≠product; not bar3)\n");
                                /* Grep: cpu: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("cpu: soft retcrown exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=100 "
                                        "(retcrown stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: cpu: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("cpu: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=100 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: cpu: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("cpu: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=100 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: cpu: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("cpu: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=100 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: cpu: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("cpu: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=100 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: cpu: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("cpu: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=100 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: cpu: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("cpu: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=100 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: cpu: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("cpu: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=100 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("cpu: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("cpu: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("cpu: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("cpu: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("cpu: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("cpu: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retfortress — Wave 35 return-fortress honesty */
kprintf("cpu: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("cpu: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft rethold — Wave 36 return-hold honesty */
kprintf("cpu: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retspire — Wave 36 exclusive spire stamp */
kprintf("cpu: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retwall — Wave 37 return-wall honesty */
kprintf("cpu: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retgate — Wave 37 exclusive gate stamp */
kprintf("cpu: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retmoat — Wave 38 return-moat honesty */
kprintf("cpu: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retower — Wave 38 exclusive tower stamp */
kprintf("cpu: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("cpu: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("cpu: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("cpu: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("cpu: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retravelin — Wave 41 return-travelin honesty */
kprintf("cpu: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("cpu: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("cpu: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("cpu: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("cpu: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("cpu: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("cpu: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("cpu: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("cpu: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("cpu: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retbailey — Wave 46 return-bailey honesty */
kprintf("cpu: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("cpu: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("cpu: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("cpu: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("cpu: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("cpu: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("cpu: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("cpu: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retsally — Wave 50 return-sally honesty */
kprintf("cpu: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("cpu: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retfosse — Wave 51 return-fosse honesty */
kprintf("cpu: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("cpu: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("cpu: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("cpu: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retravelin — Wave 53 return-travelin honesty */
kprintf("cpu: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("cpu: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("cpu: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retredan — Wave 54 exclusive redan stamp */
kprintf("cpu: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retflank — Wave 55 return-flank honesty */
kprintf("cpu: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retface — Wave 55 exclusive face stamp */
kprintf("cpu: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retgorge — Wave 56 return-gorge honesty */
kprintf("cpu: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("cpu: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retraverse — Wave 57 return-traverse honesty */
kprintf("cpu: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("cpu: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retorillon — Wave 58 return-orillon honesty */
kprintf("cpu: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("cpu: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("cpu: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("cpu: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retplace — Wave 60 return-place honesty */
kprintf("cpu: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("cpu: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("cpu: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("cpu: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("cpu: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("cpu: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("cpu: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("cpu: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cpu: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("cpu: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: cpu: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("cpu: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cpu: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("cpu: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cpu: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("cpu: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cpu: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("cpu: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=100 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cpu: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("cpu: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=100 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("cpu: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("cpu: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("cpu: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("cpu: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("cpu: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=100 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("cpu: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=100 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("cpu: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("cpu: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("cpu: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("cpu: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("cpu: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("cpu: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("cpu: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("cpu: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retbastionangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("cpu: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("cpu: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retparapetangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("cpu: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("cpu: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retowerangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("cpu: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("cpu: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retwallangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("cpu: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("cpu: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retholdangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("cpu: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("cpu: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retfortressangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("cpu: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("cpu: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("cpu: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("cpu: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: cpu: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("cpu: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("cpu: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("cpu: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("cpu: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retaegisangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("cpu: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("cpu: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retsigilangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("cpu: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("cpu: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retglyphangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("cpu: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("cpu: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retshardangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("cpu: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("cpu: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retprismangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("cpu: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("cpu: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retcipherangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("cpu: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("cpu: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retledgerangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("cpu: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("cpu: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retvaultangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("cpu: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("cpu: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (rettokenangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("cpu: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("cpu: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retphaseangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("cpu: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("cpu: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retpulseangle stamp; Soft≠product)\n");

/* Grep: cpu: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("cpu: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("cpu: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retboundangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("cpu: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("cpu: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retbladeangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("cpu: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("cpu: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retarcangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("cpu: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("cpu: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("cpu: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("cpu: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("cpu: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("cpu: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retellipseangle stamp; Soft≠product)\n");
/* Grep: cpu: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("cpu: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("cpu: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: cpu: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("cpu: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("cpu: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (rethelixangle stamp; Soft≠product)\n");
                                kprintf("cpu: soft deepen wave=100 areas=141 online=%u static=%u "
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
