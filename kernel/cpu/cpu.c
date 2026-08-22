/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Percpu pool + GS_BASE. Layout of struct gj_cpu is an asm contract -
 * see cpu.h; do not reorder leading fields (sched / SYSCALL / swapgs).
 *
 * Lean soft residual (this unit only; Soft!=product; G-AC-1; no stamp storms):
 *   Static vs dyn online, OOM/reject/idempotent, max online id, last dyn
 *   PA/pages, GS sanity, publish class split, greppable cpu_soft_log.
 *   C0 residual integrity (derived, Soft only): walk/gs/kgs/id/dyn match,
 *   kstack align+nz, cr3 nz, publish==walk, BSP online + CR3 live shadow.
 *   Greppable prefix-stable serial (no version stamp; no multi-kprintf flood):
 *     cpu: soft PASS|UP|PARTIAL|DEGRADED  - primary inventory (field-stable)
 *     cpu: soft counters|probe            - complementary tallies
 *     cpu: soft slot=                     - published slots (cap-bounded walk)
 *     cpu: soft residual                  - lean residual honesty Soft!=product
 *     cpu: soft residual lean             - integrity rollup (C0 residual)
 *     cpu: soft residual lean deepen      - integrity-first deepen (C0 STRONGER)
 *     cpu: soft reject|idempotent         - event lines on bring-up edges
 *   Soft only - never hard-gates product bring-up. Dual MIT OR Apache-2.0.
 *   Dual DoD A/B remain OPEN (this unit is C0 residual, not DoD close).
 * Grep: cpu: soft / cpu: soft residual / cpu: soft residual lean
 * Grep: cpu: soft residual lean deepen / Soft!=product / soft_ne_product
 * Grep: G-AC-1 / storm=0 / stamp_free=1 / dual_dod
 */
#include <gj/cpu.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

#define MSR_FS_BASE        0xC0000100u
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
 * Complementary soft counters (file-local residual).
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

/** Soft: mark slot published - bump max online id. */
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
 * Soft: count online slots with bad kstack top (zero or not 16-byte aligned).
 * After thread install, KERNEL_RSP may leave aSyscallStack - still must be
 * non-zero and 16-byte aligned (GJ_SYSCALL_STACK_SIZE / ABI). Soft only.
 */
static u32
cpu_soft_count_kstack_bad(void)
{
    u32 i;
    u32 u32N = 0;
    struct gj_cpu *pCpu;
    u64 u64Rsp;

    for (i = 0; i < GJ_CPU_STATIC_MAX; i++) {
        if (g_aCpus[i].u32Online == 0) {
            continue;
        }
        u64Rsp = g_aCpus[i].u64KernelRsp;
        if (u64Rsp == 0 || (u64Rsp & 0xfull) != 0) {
            u32N++;
        }
    }
    for (i = GJ_CPU_STATIC_MAX; i < GJ_MAX_CPUS; i++) {
        pCpu = g_apCpuDyn[i];
        if (pCpu == NULL || pCpu->u32Online == 0) {
            continue;
        }
        u64Rsp = pCpu->u64KernelRsp;
        if (u64Rsp == 0 || (u64Rsp & 0xfull) != 0) {
            u32N++;
        }
    }
    return u32N;
}

/**
 * Soft: count online slots with zero CR3 shadow (should be non-zero once
 * published after cpu_init_* captures live CR3). Soft only; not a gate.
 */
static u32
cpu_soft_count_cr3_zero(void)
{
    u32 i;
    u32 u32N = 0;
    struct gj_cpu *pCpu;

    for (i = 0; i < GJ_CPU_STATIC_MAX; i++) {
        if (g_aCpus[i].u32Online != 0 && g_aCpus[i].u64Cr3 == 0) {
            u32N++;
        }
    }
    for (i = GJ_CPU_STATIC_MAX; i < GJ_MAX_CPUS; i++) {
        pCpu = g_apCpuDyn[i];
        if (pCpu != NULL && pCpu->u32Online != 0 && pCpu->u64Cr3 == 0) {
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
 * Used by GS sanity probe - walks pool, not hot path.
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
     * enter_user: swapgs -> GS=0, KERNEL_GS=percpu; SYSCALL: swapgs -> GS=percpu.
     */
    if (pCpu == NULL) {
        return;
    }
    cpu_soft_inc(&g_u32SoftGsInit);
    wrmsr(MSR_GS_BASE, (u64)(gj_vaddr_t)pCpu);
    wrmsr(MSR_KERNEL_GS_BASE, 0);
}

#define CR0_MP           (1ull << 1)
#define CR0_EM           (1ull << 2)
#define CR0_NE           (1ull << 5)
#define CR4_OSFXSR       (1ull << 9)
#define CR4_OSXMMEXCPT   (1ull << 10)
#define CR4_OSXSAVE      (1ull << 18)
#define CPUID1_ECX_XSAVE (1u << 26) /* CPU XSAVE/XSETBV; OSXSAVE (27) is CR4 */
#define CPUID1_ECX_AVX   (1u << 28)

/**
 * Enable x87 + SSE2 on this CPU (CR0/CR4 + fninit).
 * After OSFXSR: if CPUID.1 XSAVE, CR4.OSXSAVE then XSETBV XCR0.
 * OpenSSL ia32cap XGETBV; Dual DoD B OPEN.
 */
static void
cpu_enable_fpu(void)
{
    u64 u64Cr0;
    u64 u64Cr4;
    u32 u32Eax;
    u32 u32Ebx;
    u32 u32Ecx;
    u32 u32Edx;
    u32 u32Xcr0Lo;
    u32 u32Xcr0Hi;

    __asm__ volatile ("mov %%cr0, %0" : "=r"(u64Cr0));
    u64Cr0 &= ~CR0_EM;
    u64Cr0 |= CR0_MP | CR0_NE;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(u64Cr0) : "memory");

    __asm__ volatile ("mov %%cr4, %0" : "=r"(u64Cr4));
    u64Cr4 |= CR4_OSFXSR | CR4_OSXMMEXCPT;
    __asm__ volatile ("mov %0, %%cr4" : : "r"(u64Cr4) : "memory");

    /* OpenSSL ia32cap XGETBV; Dual DoD B OPEN. */
    __asm__ volatile ("cpuid"
                      : "=a"(u32Eax), "=b"(u32Ebx), "=c"(u32Ecx), "=d"(u32Edx)
                      : "a"(1u), "c"(0u));
    (void)u32Eax;
    (void)u32Ebx;
    (void)u32Edx;
    /* XSAVE (26), not OSXSAVE (27): bit 27 mirrors CR4; writing OSXSAVE
     * without XSAVE #GPs. */
    if ((u32Ecx & CPUID1_ECX_XSAVE) != 0) {
        u64Cr4 |= CR4_OSXSAVE;
        __asm__ volatile ("mov %0, %%cr4" : : "r"(u64Cr4) : "memory");
        u32Xcr0Lo = 0x3u; /* x87 + SSE */
        if ((u32Ecx & CPUID1_ECX_AVX) != 0) {
            u32Xcr0Lo = 0x7u; /* x87 + SSE + YMM */
        }
        u32Xcr0Hi = 0u;
        __asm__ volatile ("xsetbv"
                          :
                          : "c"(0u), "a"(u32Xcr0Lo), "d"(u32Xcr0Hi)
                          : "memory");
    }

    __asm__ volatile ("fninit" : : : "memory");
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
    /* Complementary soft counters (keep bsp_init/log sticky across reset). */
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
    cpu_enable_fpu();
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
            /* pmm_alloc_pages -> 0 on OOM: leave AP without GS percpu. */
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
    cpu_enable_fpu();
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
    u32 u32StaticHead;
    u32 u32DynCeil;
    u32 u32DynHead;
    u32 u32GsBaseMatch;
    u64 u64GsNow;
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
     *   UP      - BSP only, no OOM/reject noise
     *   PASS    - multi-CPU published, no OOM
     *   PARTIAL - some online + OOM or dyn alloc without matching online
     *   DEGRADED- OOM or reject with only BSP / inconsistent online
     * Primary fields/order frozen for smoke greps - do not reorder.
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
     *   cpu: soft PASS|UP|PARTIAL|DEGRADED online=... static=... dyn=... ...
     * Field order stable - complementary data goes on lines below.
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
     * Complementary greppable inventory - never alters PASS line.
     *   cpu: soft counters ...
     *   cpu: soft probe ...
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
        /* Greppable: cpu: soft slot= - field order stable. */
        kprintf("cpu: soft slot=%u kind=%u kstack=0x%lx cr3=0x%lx thr=%p "
                "online=%u\n",
                i, u32Kind, (unsigned long)pCpu->u64KernelRsp,
                (unsigned long)pCpu->u64Cr3, (void *)pCpu->pCurrent,
                pCpu->u32Online);
    }

    /*
     * Lean soft residual (Soft!=product; G-AC-1; no stamp storms; no version
     * stamp). Honesty + integrity - never reshapes primary PASS|counters|
     * probe|slot. Dual MIT OR Apache-2.0. Dual DoD A/B OPEN (C0 residual).
     * greppable: cpu: soft residual | residual lean | residual lean deepen
     * greppable: Soft!=product | storm=0 | stamp_free=1
     */
    {
        u32 u32DynMatch;
        u32 u32EdgeRej;
        u32 u32Integrity;
        u32 u32PoolOccBp;
        u32 u32PublishSum;
        u32 u32BspOnline;
        u32 u32Cr3LiveMatch;
        u32 u32KstackBad;
        u32 u32Cr3Zero;
        u32 u32KstackOk;
        u32 u32Cr3NzOk;
        u32 u32PublishMatch;
        u32 u32RejectClassOk;
        u32 u32IntegrityDeep;

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
        u64GsNow = rdmsr(MSR_GS_BASE);
        u32GsBaseMatch = cpu_soft_va_is_published(u64GsNow) ? 1u : 0u;

        /*
         * Residual integrity (derived; Soft only; C0 STRONGER):
         *   dyn_match      - dyn_alloc == dyn_online (PARTIAL root cause)
         *   edge_rej       - zero_pages + null_va (non-bsp/oob reject edges)
         *   kstack_ok      - all online KERNEL_RSP nz + 16-byte aligned
         *   cr3_nz_ok      - all online CR3 shadows non-zero
         *   publish_match  - publish static/dyn counters match walk online
         *   reject_class   - rej_bsp+rej_oob+edge_rej <= reject (class sum)
         *   integrity      - walk+gs+kgs+id+dyn+kstack+cr3_nz clean
         *   integrity_deep - integrity + bsp + cr3_live + publish + reject
         *   pool_occ_bp    - online / max_cpus * 10000 (basis points)
         *   bsp_online     - slot 0 published (BSP path)
         *   cr3_live       - BSP shadow matches live CR3 (no shootdown claim)
         */
        u32DynMatch =
            (stSoft.u32DynAlloc == stSoft.u32DynOnline) ? 1u : 0u;
        u32EdgeRej = g_u32SoftZeroPages + g_u32SoftNullVa;
        if (u32EdgeRej < g_u32SoftZeroPages) {
            u32EdgeRej = 0xffffffffu; /* saturate on wrap */
        }
        u32KstackBad = cpu_soft_count_kstack_bad();
        u32Cr3Zero = cpu_soft_count_cr3_zero();
        u32KstackOk = (u32KstackBad == 0) ? 1u : 0u;
        u32Cr3NzOk = (u32Cr3Zero == 0) ? 1u : 0u;
        u32PublishSum = g_u32SoftPublishStatic + g_u32SoftPublishDyn;
        u32PublishMatch =
            (g_u32SoftPublishStatic == stSoft.u32StaticOnline &&
             g_u32SoftPublishDyn == stSoft.u32DynOnline)
                ? 1u
                : 0u;
        /* Class sum may under-count if only total reject sticky grew. */
        {
            u32 u32ClassSum = g_u32SoftRejectBsp + g_u32SoftRejectOob +
                              u32EdgeRej;
            if (u32ClassSum < g_u32SoftRejectBsp) {
                u32ClassSum = 0xffffffffu;
            }
            u32RejectClassOk =
                (u32ClassSum <= stSoft.u32Reject) ? 1u : 0u;
        }
        u32BspOnline =
            (g_aCpus[0].u32Online != 0 && g_aCpus[0].u32CpuId == 0) ? 1u
                                                                    : 0u;
        u32Cr3LiveMatch =
            (g_aCpus[0].u32Online != 0 &&
             g_aCpus[0].u64Cr3 == cpu_read_cr3())
                ? 1u
                : 0u;
        u32Integrity = (u32WalkMatch != 0 && stSoft.u32GsSane != 0 &&
                        u32GsBaseMatch != 0 && u32KgsZero != 0 &&
                        u32IdMis == 0 && u32DynMatch != 0 &&
                        u32KstackOk != 0 && u32Cr3NzOk != 0)
                           ? 1u
                           : 0u;
        u32IntegrityDeep =
            (u32Integrity != 0 && u32BspOnline != 0 &&
             u32Cr3LiveMatch != 0 && u32PublishMatch != 0 &&
             u32RejectClassOk != 0)
                ? 1u
                : 0u;
        if (stSoft.u32MaxCpus != 0) {
            u32PoolOccBp =
                (stSoft.u32Online * 10000u) / stSoft.u32MaxCpus;
        } else {
            u32PoolOccBp = 0;
        }

        /* Grep: cpu: soft residual | Soft!=product | G-AC-1 | stamp_free */
        kprintf("cpu: soft residual verdict=%s online=%u static=%u dyn=%u "
                "dyn_alloc=%u oom=%u reject=%u idem=%u max_id=%u "
                "static_head=%u dyn_head=%u dyn_ceil=%u "
                "gs_sane=%u base_match=%u kgs_zero=%u walk_match=%u "
                "pub_static=%u pub_dyn=%u hhdm=%u ident=%u "
                "rej_bsp=%u rej_oob=%u dyn_ptrs=%u pages_per=%u "
                "logs=%u id_mis=%u dyn_match=%u edge_rej=%u "
                "kstack_ok=%u cr3_nz_ok=%u publish_match=%u "
                "reject_class_ok=%u integrity=%u integrity_deep=%u "
                "pool_occ_bp=%u bsp_online=%u cr3_live=%u "
                "soft=1 product=0 Soft!=product soft_ne_product=1 "
                "residual!=product storm=0 stamp_free=1 G-AC-1 "
                "no_version_stamp=1 dual=MIT_OR_Apache-2.0 "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "(percpu GS pool; not multi-CPU product gate; not DoD close)\n",
                szVerdict, stSoft.u32Online, stSoft.u32StaticOnline,
                stSoft.u32DynOnline, stSoft.u32DynAlloc, stSoft.u32Oom,
                stSoft.u32Reject, stSoft.u32Idempotent,
                stSoft.u32MaxOnlineId, u32StaticHead, u32DynHead,
                u32DynCeil, stSoft.u32GsSane, u32GsBaseMatch, u32KgsZero,
                u32WalkMatch, g_u32SoftPublishStatic, g_u32SoftPublishDyn,
                g_u32SoftDynHhdm, g_u32SoftDynIdent, g_u32SoftRejectBsp,
                g_u32SoftRejectOob, u32DynPtrs, u32PagesPer,
                g_u32SoftLogN, u32IdMis, u32DynMatch, u32EdgeRej,
                u32KstackOk, u32Cr3NzOk, u32PublishMatch,
                u32RejectClassOk, u32Integrity, u32IntegrityDeep,
                u32PoolOccBp, u32BspOnline, u32Cr3LiveMatch);

        /*
         * Grep: cpu: soft residual lean
         * C0 residual: integrity-first rollup (not a stamp storm).
         * Soft!=product; never gates product bring-up; Dual DoD OPEN.
         */
        kprintf("cpu: soft residual lean verdict=%s integrity=%u "
                "integrity_deep=%u walk_match=%u gs_sane=%u base_match=%u "
                "kgs_zero=%u id_mis=%u dyn_match=%u kstack_ok=%u "
                "cr3_nz_ok=%u publish_match=%u reject_class_ok=%u "
                "bsp_online=%u cr3_live=%u online=%u static_head=%u "
                "dyn_head=%u dyn_ceil=%u oom=%u reject=%u edge_rej=%u "
                "publish=%u dyn_reuse=%u pool_occ_bp=%u "
                "soft=1 product=0 soft_ne_product=1 residual!=product "
                "storm=0 stamp_free=1 G-AC-1 no_version_stamp=1 "
                "dual=MIT_OR_Apache-2.0 dual_dod_a=OPEN dual_dod_b=OPEN "
                "(Soft!=product; percpu residual; not product multi-CPU)\n",
                szVerdict, u32Integrity, u32IntegrityDeep, u32WalkMatch,
                stSoft.u32GsSane, u32GsBaseMatch, u32KgsZero, u32IdMis,
                u32DynMatch, u32KstackOk, u32Cr3NzOk, u32PublishMatch,
                u32RejectClassOk, u32BspOnline, u32Cr3LiveMatch,
                stSoft.u32Online, u32StaticHead, u32DynHead, u32DynCeil,
                stSoft.u32Oom, stSoft.u32Reject, u32EdgeRej, u32PublishSum,
                g_u32SoftDynReuse, u32PoolOccBp);

        /*
         * Grep: cpu: soft residual lean deepen
         * C0 STRONGER residual deepen: compact integrity lamp set only.
         * No wave= / ret*angle / image version. Dual DoD A/B stay OPEN.
         */
        kprintf("cpu: soft residual lean deepen residual_lean=1 "
                "integrity=%u integrity_deep=%u kstack_ok=%u "
                "cr3_nz_ok=%u publish_match=%u reject_class_ok=%u "
                "kstack_bad=%u cr3_zero=%u walk_match=%u dyn_match=%u "
                "gs_sane=%u base_match=%u kgs_zero=%u bsp_online=%u "
                "cr3_live=%u id_mis=%u online=%u oom=%u reject=%u "
                "soft=1 product=0 soft_ne_product=1 residual!=product "
                "storm=0 stamp_free=1 G-AC-1 no_version_stamp=1 "
                "dual=MIT_OR_Apache-2.0 dual_dod_a=OPEN dual_dod_b=OPEN "
                "(C0 cpu residual deepen; Soft!=product; not DoD close)\n",
                u32Integrity, u32IntegrityDeep, u32KstackOk, u32Cr3NzOk,
                u32PublishMatch, u32RejectClassOk, u32KstackBad,
                u32Cr3Zero, u32WalkMatch, u32DynMatch, stSoft.u32GsSane,
                u32GsBaseMatch, u32KgsZero, u32BspOnline, u32Cr3LiveMatch,
                u32IdMis, stSoft.u32Online, stSoft.u32Oom,
                stSoft.u32Reject);
    }
}

struct gj_cpu *
cpu_current(void)
{
    u64 u64Gs;

    __asm__ volatile ("mov %%gs:0, %0" : "=r"(u64Gs));
    /*
     * First field is u64KernelRsp - not a self pointer.
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
     * First entry (trampoline) still needs KERNEL_RSP before enter_user -
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

void
cpu_set_fs_base(u64 u64Base)
{
    wrmsr(MSR_FS_BASE, u64Base);
}
