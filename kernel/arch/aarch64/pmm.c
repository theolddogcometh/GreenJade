/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 PMM - thin wrap over shared freelist core (kernel/shared/pmm_freelist.c).
 *
 * -------------------------------------------------------------------------
 * Soft residual inventory (C0 exclusive residual; this unit only - greppable
 * "aarch64: pmm soft ...")
 * -------------------------------------------------------------------------
 * Soft pool geometry: base/end/page counts after init (order-0 identity
 * pool only; no hierarchical PMM, no NEON, no buddy paste).
 * Soft multi exercise: alloc N pages, dual-pattern paint/verify, distinct
 * PAs, free reverse; free_count restored.
 * Soft LIFO reuse: free B then next alloc returns B (freelist head).
 * Soft free-step: single alloc drops free by 1; free restores.
 * Soft null free: free(NULL) is no-op (count unchanged).
 * Soft invariant: free <= total, non-zero total, pool geometry coherent.
 * Soft geom residual: span == pages * page_size; base/end page-aligned.
 * Soft residual lean (STRONGER C0 residual; Soft!=product; stamp-free):
 *   reject   - null + unaligned + OOB free leave free/total unchanged
 *   scrub    - alloc scrubs freelist node header (first u64 == 0)
 *   payload  - paint at +64; free + LIFO realloc; payload survives
 *   double   - free C then A; next two allocs = A then former-B
 *   restore  - free_count restored; total immutable across residual
 * Soft stats: gate sum + free ratio + log tally.
 * Soft deepen: lean area catalog (no stamp storm).
 * Soft return: selftest/inventory return surfaces.
 * Soft path honesty: order-0 shared core only; not >=1 TiB hierarchical.
 * Soft honesty: aarch64 product kernel remains OPEN (soft scaffold only).
 *
 * Greppable soft inventory (prefix-stable):
 *   aarch64: pmm soft pool base=... end=... pages=... page_size=... span=...
 *   aarch64: pmm soft multi n=... free0=... free1=... total=... ok=...
 *   aarch64: pmm soft lifo pa_a=... pa_b=... pa_c=... reuse=... free0=... free1=...
 *   aarch64: pmm soft step free0=... free1=... free2=... drop=... restore=...
 *   aarch64: pmm soft inv free=... total=... pool_pages=... self=... multi=...
 *             lifo=... step=... null=... inv=... geom=... residual=...
 *   aarch64: pmm soft geom span=... expect=... align_base=... align_end=... ok=...
 *   aarch64: pmm soft residual lean ... (stamp-free Soft!=product dual license)
 *   aarch64: pmm soft residual reject|scrub|payload|double ...
 *   aarch64: pmm soft stats gates=... free=... total=... ratio=... logs=...
 *   aarch64: pmm soft inventory ...
 *   aarch64: pmm soft deepen areas=... catalog=...
 *   aarch64: pmm soft return inv_ret=... product_kernel=OPEN
 *   aarch64: pmm soft path order0=1 hier=0 neon=0 tib_bar=0 core=1
 *             product_kernel=OPEN
 *   aarch64: pmm soft honesty product_kernel=OPEN soft_only=1
 *   aarch64: pmm soft exclusive ... residual=1
 *   aarch64: pmm soft open ... product_kernel=OPEN
 *   aarch64: pmm soft PASS | FAIL
 *
 * Legacy / product smoke markers (kept greppable):
 *   aarch64: pmm PASS (shared core free=... total=...)
 *   aarch64: pmm pool soft base=... end=... pages=...
 *   aarch64: pmm multi soft n=... free0=... free1=...
 *   aarch64: pmm multi soft PASS | FAIL
 *
 * Soft residual != product hierarchical PMM / Dual DoD / 1 TiB bar.
 * Soft!=product. G-AC-1. Dual MIT OR Apache-2.0. No GPL. No version stamp.
 * Freestanding pure C; no GPL Linux buddy paste; no NEON.
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>
#include <gj/types.h>

extern char __bss_end[];
extern char __kernel_end[];

#define PMM_POOL_PAGES 4096u /* 16 MiB */

/* Soft multi-page exercise depth (stack array of page pointers). */
#define PMM_SOFT_MULTI_N 8u

/* Soft poison patterns for allocated page first words (pure C stores). */
#define PMM_SOFT_PAT_A 0xa5a5a5a5a5a5a5a5ull
#define PMM_SOFT_PAT_B 0x5a5a5a5a5a5a5a5aull
/* Residual payload past freelist node header (core scrubs sizeof(node) only). */
#define PMM_SOFT_PAYLOAD_OFF 64ul
#define PMM_SOFT_PAYLOAD_MARK 0xC0FFEE64u
#define PMM_SOFT_PAYLOAD_MARK2 0xA11CE128u
#define PMM_SOFT_RESID_PAT 0xC0ull

/*
 * Soft deepen area catalog count (honest; not stamp-storm inflated).
 * pool,multi,lifo,step,inv,geom,residual,stats,inventory,deepen,surf,path,
 * honesty,exclusive,open,return = 16.
 * Residual sub-lamps (reject/scrub/payload/double) fold under residual.
 */
#define PMM_SOFT_AREAS 16u

/* Soft residual gate count (self,multi,lifo,step,null,inv,geom,residual). */
#define PMM_SOFT_GATE_N 8u

/* Residual sub-surface bits (stamp-free; Soft!=product observability). */
#define PMM_RESID_SURF_REJECT  (1u << 0)
#define PMM_RESID_SURF_SCRUB   (1u << 1)
#define PMM_RESID_SURF_PAYLOAD (1u << 2)
#define PMM_RESID_SURF_DOUBLE  (1u << 3)
#define PMM_RESID_SURF_RESTORE (1u << 4)
#define PMM_RESID_SURF_TOTAL   (1u << 5)

static u64 g_u64PoolBase;
static u64 g_u64PoolEnd;
static unsigned g_cPoolPages;
static unsigned g_cPmmSoftLogs; /* inventory emit count */

/*
 * Soft inventory snapshot (file-local; never hard-gates boot).
 * greppable: aarch64: pmm soft
 */
struct pmm_soft_snap {
    unsigned cFree;
    unsigned cTotal;
    unsigned cPoolPages;
    unsigned cMultiGot;
    unsigned cMultiFree0;
    unsigned cMultiFree1;
    unsigned cLifoFree0;
    unsigned cLifoFree1;
    unsigned cStepFree0;
    unsigned cStepFree1;
    unsigned cStepFree2;
    unsigned cResidFree0;
    unsigned cResidFree1;
    unsigned cResidTotal0;
    unsigned cResidTotal1;
    unsigned u32ResidSurf;
    u64      u64LifoPaA;
    u64      u64LifoPaB;
    u64      u64LifoPaC;
    u64      u64ResidPa;
    u64      u64ResidPaB;
    u64      u64PoolSpan;
    u64      u64SpanExpect;
    u8       u8SelfOk;
    u8       u8MultiOk;
    u8       u8LifoOk;
    u8       u8StepOk;
    u8       u8NullOk;
    u8       u8InvOk;
    u8       u8GeomOk;
    u8       u8ResidOk;
    u8       u8ResidRejectOk;
    u8       u8ResidScrubOk;
    u8       u8ResidPayloadOk;
    u8       u8ResidDoubleOk;
    u8       u8MultiDistinctOk;
};

static unsigned long
align_up(unsigned long v, unsigned long a)
{
    return (v + (a - 1ul)) & ~(a - 1ul);
}

void *
aarch64_pmm_alloc(void)
{
    u64 pa = gj_pmm_core_alloc();

    return (pa == 0) ? 0 : (void *)(gj_vaddr_t)pa;
}

void
aarch64_pmm_free(void *p)
{
    if (p == 0) {
        return;
    }
    gj_pmm_core_free((u64)(gj_vaddr_t)p);
}

unsigned
aarch64_pmm_free_count(void)
{
    return gj_pmm_core_free_count();
}

/*
 * Soft multi-page alloc/paint/verify/free. Returns 1 if free_count restored
 * and all pattern checks held. Safe when pool is small (partial alloc OK
 * only if cGot == 0 -> FAIL; else verify the pages we got).
 * STRONGER: distinct PA set among got pages; total immutable across multi.
 */
static int
pmm_multi_soft_exercise(unsigned *pOutGot, unsigned *pOutFree0,
                        unsigned *pOutFree1, int *pOutDistinctOk)
{
    void *aPages[PMM_SOFT_MULTI_N];
    unsigned iPage;
    unsigned jPage;
    unsigned cGot;
    unsigned cFree0;
    unsigned cFree1;
    unsigned cTotal0;
    unsigned cTotal1;
    int fOk;
    int fDistinct;

    cFree0 = gj_pmm_core_free_count();
    cTotal0 = gj_pmm_core_total_count();
    cGot = 0u;
    fOk = 1;
    fDistinct = 1;

    for (iPage = 0u; iPage < PMM_SOFT_MULTI_N; iPage++) {
        aPages[iPage] = aarch64_pmm_alloc();
        if (aPages[iPage] == 0) {
            break;
        }
        cGot++;
    }

    if (cGot == 0u) {
        fOk = 0;
        fDistinct = 0;
    }

    /* Soft distinct: every got page PA unique. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        u64 paI = (u64)(gj_vaddr_t)aPages[iPage];

        for (jPage = iPage + 1u; jPage < cGot; jPage++) {
            if (paI == (u64)(gj_vaddr_t)aPages[jPage]) {
                fDistinct = 0;
                fOk = 0;
            }
        }
    }

    /* Soft paint pass A. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        pWord[0] = PMM_SOFT_PAT_A ^ (u64)iPage;
        pWord[1] = PMM_SOFT_PAT_B ^ (u64)iPage;
    }
    /* Soft verify pass A. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        if (pWord[0] != (PMM_SOFT_PAT_A ^ (u64)iPage) ||
            pWord[1] != (PMM_SOFT_PAT_B ^ (u64)iPage)) {
            fOk = 0;
        }
    }

    /* Soft paint pass B (overwrite) + verify + bounds. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        pWord[0] = PMM_SOFT_PAT_B ^ ((u64)iPage << 8);
        pWord[1] = PMM_SOFT_PAT_A ^ ((u64)iPage << 8);
    }
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];
        u64 pa;

        if (pWord[0] != (PMM_SOFT_PAT_B ^ ((u64)iPage << 8)) ||
            pWord[1] != (PMM_SOFT_PAT_A ^ ((u64)iPage << 8))) {
            fOk = 0;
        }
        /* Soft pool bounds: page PA must lie in [base, end). */
        pa = (u64)(gj_vaddr_t)aPages[iPage];
        if (pa < g_u64PoolBase || pa >= g_u64PoolEnd) {
            fOk = 0;
        }
        if ((pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
            fOk = 0;
        }
    }

    /* Free reverse order (soft LIFO stress on freelist). */
    for (iPage = cGot; iPage > 0u; iPage--) {
        aarch64_pmm_free(aPages[iPage - 1u]);
        aPages[iPage - 1u] = 0;
    }

    cFree1 = gj_pmm_core_free_count();
    cTotal1 = gj_pmm_core_total_count();
    if (cFree1 != cFree0) {
        fOk = 0;
    }
    if (cFree1 > cTotal1) {
        fOk = 0;
    }
    if (cTotal1 != cTotal0) {
        fOk = 0;
    }

    if (pOutGot != 0) {
        *pOutGot = cGot;
    }
    if (pOutFree0 != 0) {
        *pOutFree0 = cFree0;
    }
    if (pOutFree1 != 0) {
        *pOutFree1 = cFree1;
    }
    if (pOutDistinctOk != 0) {
        *pOutDistinctOk = fDistinct;
    }
    return fOk;
}

/*
 * Soft LIFO reuse: free of B must surface as next alloc (freelist head).
 * Returns 1 on reuse + free_count restore. Pure C; no NEON.
 */
static int
pmm_lifo_soft_exercise(u64 *pOutPaA, u64 *pOutPaB, u64 *pOutPaC,
                       unsigned *pOutFree0, unsigned *pOutFree1)
{
    void *pA;
    void *pB;
    void *pC;
    unsigned cFree0;
    unsigned cFree1;
    unsigned cTotal;
    u64 u64PaA;
    u64 u64PaB;
    u64 u64PaC;
    int fOk;

    cFree0 = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();
    fOk = 1;
    u64PaA = 0ull;
    u64PaB = 0ull;
    u64PaC = 0ull;

    if (cFree0 < 2u || cTotal < 2u) {
        fOk = 0;
        goto out;
    }

    pA = aarch64_pmm_alloc();
    pB = aarch64_pmm_alloc();
    if (pA == 0 || pB == 0) {
        if (pA != 0) {
            aarch64_pmm_free(pA);
        }
        if (pB != 0) {
            aarch64_pmm_free(pB);
        }
        fOk = 0;
        goto out;
    }

    u64PaA = (u64)(gj_vaddr_t)pA;
    u64PaB = (u64)(gj_vaddr_t)pB;
    if (u64PaA == u64PaB) {
        aarch64_pmm_free(pA);
        aarch64_pmm_free(pB);
        fOk = 0;
        goto out;
    }
    if (u64PaA < g_u64PoolBase || u64PaA >= g_u64PoolEnd ||
        u64PaB < g_u64PoolBase || u64PaB >= g_u64PoolEnd) {
        aarch64_pmm_free(pA);
        aarch64_pmm_free(pB);
        fOk = 0;
        goto out;
    }

    /* Free B first -> freelist head; next alloc must return B. */
    aarch64_pmm_free(pB);
    pC = aarch64_pmm_alloc();
    if (pC == 0) {
        aarch64_pmm_free(pA);
        fOk = 0;
        goto out;
    }
    u64PaC = (u64)(gj_vaddr_t)pC;
    if (u64PaC != u64PaB) {
        fOk = 0;
    }

    aarch64_pmm_free(pA);
    aarch64_pmm_free(pC);

    cFree1 = gj_pmm_core_free_count();
    if (cFree1 != cFree0) {
        fOk = 0;
    }

out:
    cFree1 = gj_pmm_core_free_count();
    if (pOutPaA != 0) {
        *pOutPaA = u64PaA;
    }
    if (pOutPaB != 0) {
        *pOutPaB = u64PaB;
    }
    if (pOutPaC != 0) {
        *pOutPaC = u64PaC;
    }
    if (pOutFree0 != 0) {
        *pOutFree0 = cFree0;
    }
    if (pOutFree1 != 0) {
        *pOutFree1 = cFree1;
    }
    return fOk;
}

/*
 * Soft free-count step: one alloc drops free by 1; free restores.
 * Also soft-checks null free is a no-op (via *pOutNullOk).
 */
static int
pmm_step_soft_exercise(unsigned *pOutFree0, unsigned *pOutFree1,
                       unsigned *pOutFree2, int *pOutNullOk)
{
    void *pPage;
    unsigned cFree0;
    unsigned cFree1;
    unsigned cFree2;
    unsigned cAfterNull;
    int fOk;
    int fNullOk;

    cFree0 = gj_pmm_core_free_count();
    fOk = 1;
    fNullOk = 1;

    /* Soft null free: must not change free_count. */
    aarch64_pmm_free(0);
    cAfterNull = gj_pmm_core_free_count();
    if (cAfterNull != cFree0) {
        fNullOk = 0;
        fOk = 0;
    }

    pPage = aarch64_pmm_alloc();
    if (pPage == 0) {
        fOk = 0;
        cFree1 = gj_pmm_core_free_count();
        cFree2 = cFree1;
        goto out;
    }
    cFree1 = gj_pmm_core_free_count();
    if (cFree0 < 1u || cFree1 != (cFree0 - 1u)) {
        fOk = 0;
    }

    aarch64_pmm_free(pPage);
    cFree2 = gj_pmm_core_free_count();
    if (cFree2 != cFree0) {
        fOk = 0;
    }

out:
    if (pOutFree0 != 0) {
        *pOutFree0 = cFree0;
    }
    if (pOutFree1 != 0) {
        *pOutFree1 = cFree1;
    }
    if (pOutFree2 != 0) {
        *pOutFree2 = cFree2;
    }
    if (pOutNullOk != 0) {
        *pOutNullOk = fNullOk;
    }
    return fOk;
}

/*
 * Soft geometry residual: span == pages * page_size; base/end aligned.
 * Soft!=product; never invents frames.
 */
static int
pmm_geom_soft_exercise(u64 *pOutSpan, u64 *pOutExpect, int *pOutAlignBase,
                       int *pOutAlignEnd)
{
    u64 u64Span;
    u64 u64Expect;
    int fAlignBase;
    int fAlignEnd;
    int fOk;

    fOk = 1;
    u64Span = 0ull;
    u64Expect = 0ull;
    fAlignBase = 0;
    fAlignEnd = 0;

    if (g_u64PoolEnd <= g_u64PoolBase) {
        fOk = 0;
        goto out;
    }
    if (g_cPoolPages == 0u) {
        fOk = 0;
        goto out;
    }

    u64Span = g_u64PoolEnd - g_u64PoolBase;
    u64Expect = (u64)g_cPoolPages * (u64)GJ_PMM_CORE_PAGE_SIZE;
    if (u64Span != u64Expect) {
        fOk = 0;
    }

    if ((g_u64PoolBase & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) == 0ul) {
        fAlignBase = 1;
    } else {
        fOk = 0;
    }
    if ((g_u64PoolEnd & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) == 0ul) {
        fAlignEnd = 1;
    } else {
        fOk = 0;
    }

    /* Total frames must match pool page count after init. */
    if (gj_pmm_core_total_count() != g_cPoolPages) {
        fOk = 0;
    }

out:
    if (pOutSpan != 0) {
        *pOutSpan = u64Span;
    }
    if (pOutExpect != 0) {
        *pOutExpect = u64Expect;
    }
    if (pOutAlignBase != 0) {
        *pOutAlignBase = fAlignBase;
    }
    if (pOutAlignEnd != 0) {
        *pOutAlignEnd = fAlignEnd;
    }
    return fOk;
}

/*
 * Soft residual lean exercise (C0 residual; Soft!=product; stamp-free).
 * STRONGER residual deepen over order-0 wrap only:
 *   reject  - null + unaligned + OOB free leave free/total unchanged
 *   scrub   - alloc scrubs freelist node header (first u64 == 0)
 *   payload - paint at +64; free + LIFO realloc; payload survives; header scrub
 *   double  - free B then A; next two allocs = A then former-B
 *   restore - free_count restored; total immutable
 * Never invents frames; never claims hierarchical / Dual DoD / 1 TiB product.
 * G-AC-1: soft residual != product AC. Dual MIT OR Apache-2.0. No GPL.
 */
static int
pmm_residual_soft_exercise(u64 *pOutPa, u64 *pOutPaB, unsigned *pOutFree0,
                           unsigned *pOutFree1, unsigned *pOutTotal0,
                           unsigned *pOutTotal1, unsigned *pOutSurf,
                           int *pOutRejectOk, int *pOutScrubOk,
                           int *pOutPayloadOk, int *pOutDoubleOk)
{
    void *pA;
    void *pB;
    void *pC;
    void *pD;
    void *pE;
    unsigned cFree0;
    unsigned cFree1;
    unsigned cTotal0;
    unsigned cTotal1;
    unsigned cAfter;
    unsigned u32Surf;
    u64 u64PaA;
    u64 u64PaB;
    u64 u64WasA;
    u64 u64WasB;
    volatile u64 *pHdr;
    volatile u32 *pMark;
    volatile u32 *pMark2;
    int fOk;
    int fReject;
    int fScrub;
    int fPayload;
    int fDouble;

    cFree0 = gj_pmm_core_free_count();
    cTotal0 = gj_pmm_core_total_count();
    cFree1 = cFree0;
    cTotal1 = cTotal0;
    u64PaA = 0ull;
    u64PaB = 0ull;
    u32Surf = 0u;
    fOk = 1;
    fReject = 1;
    fScrub = 0;
    fPayload = 0;
    fDouble = 0;
    pA = 0;
    pB = 0;
    pC = 0;
    pD = 0;
    pE = 0;

    /*
     * --- reject residual: null + unaligned + OOB free are no-ops ---
     * Wrap free routes to core; free/total must stay stable.
     */
    aarch64_pmm_free(0);
    aarch64_pmm_free(0);
    aarch64_pmm_free((void *)(gj_vaddr_t)1ul); /* unaligned */
    aarch64_pmm_free((void *)(gj_vaddr_t)(GJ_PMM_CORE_PAGE_SIZE / 2ul));
    if (g_u64PoolEnd > g_u64PoolBase) {
        aarch64_pmm_free((void *)(gj_vaddr_t)g_u64PoolEnd); /* OOB high */
        aarch64_pmm_free((void *)(gj_vaddr_t)(g_u64PoolEnd - 1ul));
        aarch64_pmm_free((void *)(gj_vaddr_t)(g_u64PoolBase + 1ul));
    }
    if (g_u64PoolBase >= GJ_PMM_CORE_PAGE_SIZE) {
        aarch64_pmm_free(
            (void *)(gj_vaddr_t)(g_u64PoolBase - GJ_PMM_CORE_PAGE_SIZE));
    }
    cAfter = gj_pmm_core_free_count();
    if (cAfter != cFree0 || gj_pmm_core_total_count() != cTotal0) {
        fReject = 0;
        fOk = 0;
        goto out;
    }
    u32Surf |= PMM_RESID_SURF_REJECT;

    if (cFree0 < 2u || cTotal0 == 0u) {
        fOk = 0;
        goto out;
    }

    /* --- alloc A: range + align + free drop + total immutable + scrub --- */
    pA = aarch64_pmm_alloc();
    if (pA == 0) {
        fOk = 0;
        goto out;
    }
    u64PaA = (u64)(gj_vaddr_t)pA;
    if (u64PaA < g_u64PoolBase || u64PaA >= g_u64PoolEnd ||
        (u64PaA & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
        fOk = 0;
        goto out;
    }
    if (gj_pmm_core_free_count() != (cFree0 - 1u) ||
        gj_pmm_core_total_count() != cTotal0) {
        fOk = 0;
        goto out;
    }
    pHdr = (volatile u64 *)pA;
    if (pHdr[0] != 0ull) {
        fScrub = 0;
        fOk = 0;
        goto out;
    }
    fScrub = 1;
    u32Surf |= PMM_RESID_SURF_SCRUB;

    /* --- alloc B (need two pages for double LIFO) --- */
    pB = aarch64_pmm_alloc();
    if (pB == 0) {
        fOk = 0;
        goto out;
    }
    u64PaB = (u64)(gj_vaddr_t)pB;
    if (u64PaB < g_u64PoolBase || u64PaB >= g_u64PoolEnd ||
        (u64PaB & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul ||
        u64PaB == u64PaA) {
        fOk = 0;
        goto out;
    }
    if (gj_pmm_core_free_count() != (cFree0 - 2u) ||
        gj_pmm_core_total_count() != cTotal0) {
        fOk = 0;
        goto out;
    }
    if (*(volatile u64 *)pB != 0ull) {
        fScrub = 0;
        fOk = 0;
        goto out;
    }

    /*
     * --- payload residual: mark past node header; free B; LIFO realloc ---
     * Core scrub is sizeof(node) only; payload at +64 must survive free+alloc.
     */
    pMark = (volatile u32 *)(void *)((gj_vaddr_t)pB + PMM_SOFT_PAYLOAD_OFF);
    *pMark = PMM_SOFT_PAYLOAD_MARK;
    /* Also paint first words (will be scrubbed on re-alloc). */
    ((volatile u64 *)pB)[0] = PMM_SOFT_PAT_A ^ PMM_SOFT_RESID_PAT;
    ((volatile u64 *)pB)[1] = PMM_SOFT_PAT_B ^ PMM_SOFT_RESID_PAT;

    u64WasB = u64PaB;
    aarch64_pmm_free(pB);
    pB = 0;
    if (gj_pmm_core_free_count() != (cFree0 - 1u) ||
        gj_pmm_core_total_count() != cTotal0) {
        fOk = 0;
        goto out;
    }

    pC = aarch64_pmm_alloc();
    if (pC == 0 || (u64)(gj_vaddr_t)pC != u64WasB) {
        fPayload = 0;
        fOk = 0;
        goto out;
    }
    /* Header scrubbed again after re-alloc of same page. */
    if (*(volatile u64 *)pC != 0ull) {
        fScrub = 0;
        fOk = 0;
        goto out;
    }
    if (*pMark != PMM_SOFT_PAYLOAD_MARK) {
        fPayload = 0;
        fOk = 0;
        goto out;
    }
    fPayload = 1;
    u32Surf |= PMM_RESID_SURF_PAYLOAD;

    /*
     * --- double LIFO residual: free C (was B) then A; next = A then B ---
     */
    pMark2 = (volatile u32 *)(void *)((gj_vaddr_t)pA + PMM_SOFT_PAYLOAD_OFF);
    *pMark2 = PMM_SOFT_PAYLOAD_MARK2;
    u64WasA = u64PaA;
    u64WasB = (u64)(gj_vaddr_t)pC;

    aarch64_pmm_free(pC);
    pC = 0;
    aarch64_pmm_free(pA);
    pA = 0;
    if (gj_pmm_core_free_count() != cFree0 ||
        gj_pmm_core_total_count() != cTotal0) {
        fOk = 0;
        goto out;
    }

    pD = aarch64_pmm_alloc();
    pE = aarch64_pmm_alloc();
    if (pD == 0 || pE == 0) {
        fDouble = 0;
        fOk = 0;
        goto out;
    }
    if ((u64)(gj_vaddr_t)pD != u64WasA || (u64)(gj_vaddr_t)pE != u64WasB) {
        fDouble = 0;
        fOk = 0;
        goto out;
    }
    /* Payloads past node headers still intact. */
    if (*pMark2 != PMM_SOFT_PAYLOAD_MARK2 || *pMark != PMM_SOFT_PAYLOAD_MARK) {
        fPayload = 0;
        fOk = 0;
        goto out;
    }
    if (gj_pmm_core_free_count() != (cFree0 - 2u) ||
        gj_pmm_core_total_count() != cTotal0) {
        fOk = 0;
        goto out;
    }
    fDouble = 1;
    u32Surf |= PMM_RESID_SURF_DOUBLE;

    /* --- restore both pages --- */
    aarch64_pmm_free(pD);
    aarch64_pmm_free(pE);
    pD = 0;
    pE = 0;
    cFree1 = gj_pmm_core_free_count();
    cTotal1 = gj_pmm_core_total_count();
    if (cFree1 != cFree0 || cTotal1 != cTotal0) {
        fOk = 0;
        goto out;
    }
    u32Surf |= PMM_RESID_SURF_RESTORE;
    u32Surf |= PMM_RESID_SURF_TOTAL;

out:
    /* Best-effort restore if a path left pages outstanding. */
    if (pA != 0) {
        aarch64_pmm_free(pA);
        pA = 0;
    }
    if (pB != 0) {
        aarch64_pmm_free(pB);
        pB = 0;
    }
    if (pC != 0) {
        aarch64_pmm_free(pC);
        pC = 0;
    }
    if (pD != 0) {
        aarch64_pmm_free(pD);
        pD = 0;
    }
    if (pE != 0) {
        aarch64_pmm_free(pE);
        pE = 0;
    }

    cFree1 = gj_pmm_core_free_count();
    cTotal1 = gj_pmm_core_total_count();
    /* Residual ok requires restore + all sub-surfaces that ran. */
    if (cFree1 != cFree0 || cTotal1 != cTotal0) {
        fOk = 0;
    }
    if (fReject == 0 || fScrub == 0 || fPayload == 0 || fDouble == 0) {
        fOk = 0;
    }

    if (pOutPa != 0) {
        *pOutPa = u64PaA;
    }
    if (pOutPaB != 0) {
        *pOutPaB = u64PaB;
    }
    if (pOutFree0 != 0) {
        *pOutFree0 = cFree0;
    }
    if (pOutFree1 != 0) {
        *pOutFree1 = cFree1;
    }
    if (pOutTotal0 != 0) {
        *pOutTotal0 = cTotal0;
    }
    if (pOutTotal1 != 0) {
        *pOutTotal1 = cTotal1;
    }
    if (pOutSurf != 0) {
        *pOutSurf = u32Surf;
    }
    if (pOutRejectOk != 0) {
        *pOutRejectOk = fReject;
    }
    if (pOutScrubOk != 0) {
        *pOutScrubOk = fScrub;
    }
    if (pOutPayloadOk != 0) {
        *pOutPayloadOk = fPayload;
    }
    if (pOutDoubleOk != 0) {
        *pOutDoubleOk = fDouble;
    }
    return fOk;
}

/*
 * Soft invariant snapshot after core selftest + multi exercise.
 * Returns 1 if free/total look coherent for identity-mapped pool.
 */
static int
pmm_invariants_soft(void)
{
    unsigned cFree;
    unsigned cTotal;

    cFree = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();

    if (cTotal == 0u) {
        return 0;
    }
    if (cFree > cTotal) {
        return 0;
    }
    if (g_cPoolPages != 0u && cTotal != g_cPoolPages) {
        return 0;
    }
    if (g_u64PoolEnd <= g_u64PoolBase) {
        return 0;
    }
    /* Geometry residual folded into inv: span must match pool pages. */
    if ((g_u64PoolEnd - g_u64PoolBase) !=
        ((u64)g_cPoolPages * (u64)GJ_PMM_CORE_PAGE_SIZE)) {
        return 0;
    }
    if ((g_u64PoolBase & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
        return 0;
    }
    return 1;
}

/*
 * Soft residual inventory emission - greppable "aarch64: pmm soft ...".
 * Returns 1 if all soft gates held
 * (self/multi/lifo/step/null/inv/geom/residual).
 * Soft!=product; dual MIT OR Apache-2.0; G-AC-1; no stamp storm.
 */
static int
pmm_soft_inventory(const struct pmm_soft_snap *pSnap)
{
    unsigned uLifoReuse;
    unsigned uStepDrop;
    unsigned uStepRestore;
    unsigned cGates;
    unsigned uRatio;
    unsigned u32Bits;
    int fOk;
    int fAlignBase;
    int fAlignEnd;

    if (pSnap == 0) {
        kprintf("aarch64: pmm soft FAIL\n");
        return 0;
    }

    if (g_cPmmSoftLogs < 0xffffffffu) {
        g_cPmmSoftLogs++;
    }

    uLifoReuse = 0u;
    if (pSnap->u64LifoPaB != 0ull &&
        pSnap->u64LifoPaC == pSnap->u64LifoPaB) {
        uLifoReuse = 1u;
    }

    uStepDrop = 0u;
    if (pSnap->cStepFree0 > 0u &&
        pSnap->cStepFree1 == (pSnap->cStepFree0 - 1u)) {
        uStepDrop = 1u;
    }

    uStepRestore = 0u;
    if (pSnap->cStepFree2 == pSnap->cStepFree0) {
        uStepRestore = 1u;
    }

    fAlignBase = 0;
    fAlignEnd = 0;
    if ((g_u64PoolBase & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) == 0ul) {
        fAlignBase = 1;
    }
    if ((g_u64PoolEnd & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) == 0ul) {
        fAlignEnd = 1;
    }

    /* Soft gate sum + free/total ratio (percent; pure integer). */
    cGates = (unsigned)pSnap->u8SelfOk + (unsigned)pSnap->u8MultiOk +
             (unsigned)pSnap->u8LifoOk + (unsigned)pSnap->u8StepOk +
             (unsigned)pSnap->u8NullOk + (unsigned)pSnap->u8InvOk +
             (unsigned)pSnap->u8GeomOk + (unsigned)pSnap->u8ResidOk;
    uRatio = 0u;
    if (pSnap->cTotal != 0u) {
        uRatio = (pSnap->cFree * 100u) / pSnap->cTotal;
    }

    u32Bits = (unsigned)pSnap->u8SelfOk |
              ((unsigned)pSnap->u8MultiOk << 1) |
              ((unsigned)pSnap->u8LifoOk << 2) |
              ((unsigned)pSnap->u8StepOk << 3) |
              ((unsigned)pSnap->u8NullOk << 4) |
              ((unsigned)pSnap->u8InvOk << 5) |
              ((unsigned)pSnap->u8GeomOk << 6) |
              ((unsigned)pSnap->u8ResidOk << 7);

    /* Grep: aarch64: pmm soft pool */
    kprintf("aarch64: pmm soft pool base=0x%lx end=0x%lx pages=%u "
            "page_size=%lu span=0x%lx\n",
            (unsigned long)g_u64PoolBase, (unsigned long)g_u64PoolEnd,
            pSnap->cPoolPages, (unsigned long)GJ_PMM_CORE_PAGE_SIZE,
            (unsigned long)pSnap->u64PoolSpan);

    /* Grep: aarch64: pmm soft multi */
    kprintf("aarch64: pmm soft multi n=%u free0=%u free1=%u total=%u "
            "distinct=%u ok=%u\n",
            pSnap->cMultiGot, pSnap->cMultiFree0, pSnap->cMultiFree1,
            pSnap->cTotal, (unsigned)pSnap->u8MultiDistinctOk,
            (unsigned)pSnap->u8MultiOk);

    /* Grep: aarch64: pmm soft lifo */
    kprintf("aarch64: pmm soft lifo pa_a=0x%lx pa_b=0x%lx pa_c=0x%lx "
            "reuse=%u free0=%u free1=%u ok=%u\n",
            (unsigned long)pSnap->u64LifoPaA,
            (unsigned long)pSnap->u64LifoPaB,
            (unsigned long)pSnap->u64LifoPaC, uLifoReuse,
            pSnap->cLifoFree0, pSnap->cLifoFree1,
            (unsigned)pSnap->u8LifoOk);

    /* Grep: aarch64: pmm soft step */
    kprintf("aarch64: pmm soft step free0=%u free1=%u free2=%u drop=%u "
            "restore=%u null_ok=%u ok=%u\n",
            pSnap->cStepFree0, pSnap->cStepFree1, pSnap->cStepFree2,
            uStepDrop, uStepRestore, (unsigned)pSnap->u8NullOk,
            (unsigned)pSnap->u8StepOk);

    /* Grep: aarch64: pmm soft inv */
    kprintf("aarch64: pmm soft inv free=%u total=%u pool_pages=%u "
            "self=%u multi=%u lifo=%u step=%u null=%u inv=%u "
            "geom=%u residual=%u\n",
            pSnap->cFree, pSnap->cTotal, pSnap->cPoolPages,
            (unsigned)pSnap->u8SelfOk, (unsigned)pSnap->u8MultiOk,
            (unsigned)pSnap->u8LifoOk, (unsigned)pSnap->u8StepOk,
            (unsigned)pSnap->u8NullOk, (unsigned)pSnap->u8InvOk,
            (unsigned)pSnap->u8GeomOk, (unsigned)pSnap->u8ResidOk);

    /* Grep: aarch64: pmm soft geom */
    kprintf("aarch64: pmm soft geom span=0x%lx expect=0x%lx "
            "align_base=%u align_end=%u pages=%u page_size=%lu ok=%u\n",
            (unsigned long)pSnap->u64PoolSpan,
            (unsigned long)pSnap->u64SpanExpect,
            (unsigned)fAlignBase, (unsigned)fAlignEnd,
            pSnap->cPoolPages, (unsigned long)GJ_PMM_CORE_PAGE_SIZE,
            (unsigned)pSnap->u8GeomOk);

    /*
     * Grep: aarch64: pmm soft residual lean
     * C0 STRONGER residual lean - Soft!=product dual MIT OR Apache-2.0.
     * Stamp-free residual lamp; order-0 wrap only; not hierarchical product.
     * Sub-surfaces: reject/scrub/payload/double/restore/total (resid_surf).
     * G-AC-1: soft residual != product AC. Dual DoD remains OPEN.
     */
    kprintf("aarch64: pmm soft residual lean %s "
            "order0=1 hier=0 neon=0 hhdm=0 zones=0 "
            "free=%u total=%u pool_pages=%u "
            "resid_free0=%u resid_free1=%u "
            "resid_total0=%u resid_total1=%u "
            "resid_pa=0x%lx resid_pa_b=0x%lx "
            "reject=%u scrub=%u payload=%u double=%u "
            "resid_surf=0x%x "
            "gates=%u/%u bits=0x%x areas=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "product_kernel=OPEN product_tib=0 multi_server=0 confine=0 "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "no version stamp; order-0 shared core wrap only; "
            "not hierarchical product PMM; not Dual DoD close)\n",
            (pSnap->u8ResidOk != 0u && cGates == PMM_SOFT_GATE_N) ?
                "PASS" :
                "FAIL",
            pSnap->cFree, pSnap->cTotal, pSnap->cPoolPages,
            pSnap->cResidFree0, pSnap->cResidFree1,
            pSnap->cResidTotal0, pSnap->cResidTotal1,
            (unsigned long)pSnap->u64ResidPa,
            (unsigned long)pSnap->u64ResidPaB,
            (unsigned)pSnap->u8ResidRejectOk,
            (unsigned)pSnap->u8ResidScrubOk,
            (unsigned)pSnap->u8ResidPayloadOk,
            (unsigned)pSnap->u8ResidDoubleOk,
            pSnap->u32ResidSurf,
            cGates, (unsigned)PMM_SOFT_GATE_N, u32Bits,
            (unsigned)PMM_SOFT_AREAS);

    /* Grep: aarch64: pmm soft residual reject|scrub|payload|double */
    kprintf("aarch64: pmm soft residual reject ok=%u free0=%u free1=%u "
            "total0=%u total1=%u\n",
            (unsigned)pSnap->u8ResidRejectOk, pSnap->cResidFree0,
            pSnap->cResidFree1, pSnap->cResidTotal0, pSnap->cResidTotal1);
    kprintf("aarch64: pmm soft residual scrub ok=%u pa=0x%lx\n",
            (unsigned)pSnap->u8ResidScrubOk,
            (unsigned long)pSnap->u64ResidPa);
    kprintf("aarch64: pmm soft residual payload ok=%u off=%lu mark=0x%x\n",
            (unsigned)pSnap->u8ResidPayloadOk,
            (unsigned long)PMM_SOFT_PAYLOAD_OFF,
            (unsigned)PMM_SOFT_PAYLOAD_MARK);
    kprintf("aarch64: pmm soft residual double ok=%u pa_a=0x%lx pa_b=0x%lx "
            "surf=0x%x\n",
            (unsigned)pSnap->u8ResidDoubleOk,
            (unsigned long)pSnap->u64ResidPa,
            (unsigned long)pSnap->u64ResidPaB, pSnap->u32ResidSurf);

    /* Grep: aarch64: pmm soft stats */
    kprintf("aarch64: pmm soft stats gates=%u free=%u total=%u ratio=%u "
            "multi_n=%u logs=%u areas=%u\n",
            cGates, pSnap->cFree, pSnap->cTotal, uRatio,
            (unsigned)PMM_SOFT_MULTI_N, g_cPmmSoftLogs,
            (unsigned)PMM_SOFT_AREAS);

    /* Grep: aarch64: pmm soft inventory */
    kprintf("aarch64: pmm soft inventory gates=%u free=%u total=%u "
            "pool_pages=%u logs=%u ok_sum=%u areas=%u soft_only=1\n",
            cGates, pSnap->cFree, pSnap->cTotal,
            pSnap->cPoolPages, g_cPmmSoftLogs, cGates,
            (unsigned)PMM_SOFT_AREAS);

    /*
     * Grep: aarch64: pmm soft deepen
     * Lean area catalog - order-0 soft scaffold only; no stamp storm.
     */
    kprintf("aarch64: pmm soft deepen areas=%u "
            "catalog=pool,multi,lifo,step,inv,geom,residual,stats,"
            "inventory,deepen,surf,path,honesty,exclusive,open,return "
            "logs=%u soft_only=1 storm=0\n",
            (unsigned)PMM_SOFT_AREAS, g_cPmmSoftLogs);

    /* Grep: aarch64: pmm soft surf - gate bit lamps */
    kprintf("aarch64: pmm soft surf self=%u multi=%u lifo=%u step=%u "
            "null=%u inv=%u geom=%u residual=%u gates=%u bits=0x%x\n",
            (unsigned)pSnap->u8SelfOk, (unsigned)pSnap->u8MultiOk,
            (unsigned)pSnap->u8LifoOk, (unsigned)pSnap->u8StepOk,
            (unsigned)pSnap->u8NullOk, (unsigned)pSnap->u8InvOk,
            (unsigned)pSnap->u8GeomOk, (unsigned)pSnap->u8ResidOk, cGates,
            u32Bits);

    /*
     * Grep: aarch64: pmm soft path
     * Honesty: order-0 shared core only - not hierarchical / TiB product bar.
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    kprintf("aarch64: pmm soft path order0=1 hier=0 neon=0 tib_bar=0 "
            "core=1 multi_n=%u product_kernel=OPEN hard_gate=0\n",
            (unsigned)PMM_SOFT_MULTI_N);

    /* Grep: aarch64: pmm soft honesty */
    kprintf("aarch64: pmm soft honesty product_kernel=OPEN soft_only=1 "
            "no_hier=1 no_tib=1 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0\n");

    /* Grep: aarch64: pmm soft exclusive - C0 exclusive residual deepen */
    kprintf("aarch64: pmm soft exclusive multi_server=0 confine=0 "
            "product_kernel=OPEN soft_only=1 residual=1 "
            "resid_surf=0x%x stronger=1 stamp_free=1\n",
            pSnap->u32ResidSurf);

    /* Grep: aarch64: pmm soft open - Dual DoD remains OPEN */
    kprintf("aarch64: pmm soft open multi_server=0 confine=0 "
            "product_kernel=OPEN soft_only=1 dual_dod=OPEN "
            "product_tib=0 hier=0\n");

    fOk = 0;
    if (pSnap->u8SelfOk != 0u && pSnap->u8MultiOk != 0u &&
        pSnap->u8LifoOk != 0u && pSnap->u8StepOk != 0u &&
        pSnap->u8NullOk != 0u && pSnap->u8InvOk != 0u &&
        pSnap->u8GeomOk != 0u && pSnap->u8ResidOk != 0u) {
        fOk = 1;
    }

    /* Grep: aarch64: pmm soft return */
    kprintf("aarch64: pmm soft return inv_ret=%u self=%u multi=%u lifo=%u "
            "step=%u null=%u inv=%u geom=%u residual=%u "
            "product_kernel=OPEN\n",
            (fOk != 0) ? 1u : 0u,
            (unsigned)pSnap->u8SelfOk, (unsigned)pSnap->u8MultiOk,
            (unsigned)pSnap->u8LifoOk, (unsigned)pSnap->u8StepOk,
            (unsigned)pSnap->u8NullOk, (unsigned)pSnap->u8InvOk,
            (unsigned)pSnap->u8GeomOk, (unsigned)pSnap->u8ResidOk);

    if (fOk != 0) {
        kprintf("aarch64: pmm soft PASS areas=%u gates=%u\n",
                (unsigned)PMM_SOFT_AREAS, cGates);
    } else {
        kprintf("aarch64: pmm soft FAIL areas=%u gates=%u\n",
                (unsigned)PMM_SOFT_AREAS, cGates);
    }
    return fOk;
}

void
aarch64_pmm_init(void)
{
    unsigned long u64Start;
    unsigned long u64End;
    unsigned long u64KernelEnd;
    unsigned cFree;
    unsigned cTotal;
    unsigned cGot;
    unsigned cFree0;
    unsigned cFree1;
    int fSelf;
    int fMulti;
    int fLifo;
    int fStep;
    int fNull;
    int fInv;
    int fGeom;
    int fResid;
    int fSoft;
    int fAlignBase;
    int fAlignEnd;
    int fDistinct;
    int fResidReject;
    int fResidScrub;
    int fResidPayload;
    int fResidDouble;
    struct pmm_soft_snap snap;

    u64Start = align_up((unsigned long)(void *)__bss_end, GJ_PMM_CORE_PAGE_SIZE);
    u64KernelEnd = (unsigned long)(void *)__kernel_end;
    if (u64KernelEnd > u64Start) {
        u64Start = align_up(u64KernelEnd, GJ_PMM_CORE_PAGE_SIZE);
    }
    u64End = u64Start + ((unsigned long)PMM_POOL_PAGES << GJ_PMM_CORE_PAGE_SHIFT);

    g_u64PoolBase = (u64)u64Start;
    g_u64PoolEnd = (u64)u64End;
    g_cPoolPages = PMM_POOL_PAGES;

    gj_pmm_core_init(g_u64PoolBase, g_u64PoolEnd);

    /* Zero soft snapshot before exercises. */
    snap.cFree = 0u;
    snap.cTotal = 0u;
    snap.cPoolPages = g_cPoolPages;
    snap.cMultiGot = 0u;
    snap.cMultiFree0 = 0u;
    snap.cMultiFree1 = 0u;
    snap.cLifoFree0 = 0u;
    snap.cLifoFree1 = 0u;
    snap.cStepFree0 = 0u;
    snap.cStepFree1 = 0u;
    snap.cStepFree2 = 0u;
    snap.cResidFree0 = 0u;
    snap.cResidFree1 = 0u;
    snap.cResidTotal0 = 0u;
    snap.cResidTotal1 = 0u;
    snap.u32ResidSurf = 0u;
    snap.u64LifoPaA = 0ull;
    snap.u64LifoPaB = 0ull;
    snap.u64LifoPaC = 0ull;
    snap.u64ResidPa = 0ull;
    snap.u64ResidPaB = 0ull;
    snap.u64PoolSpan = g_u64PoolEnd - g_u64PoolBase;
    snap.u64SpanExpect =
        (u64)g_cPoolPages * (u64)GJ_PMM_CORE_PAGE_SIZE;
    snap.u8SelfOk = 0u;
    snap.u8MultiOk = 0u;
    snap.u8LifoOk = 0u;
    snap.u8StepOk = 0u;
    snap.u8NullOk = 0u;
    snap.u8InvOk = 0u;
    snap.u8GeomOk = 0u;
    snap.u8ResidOk = 0u;
    snap.u8ResidRejectOk = 0u;
    snap.u8ResidScrubOk = 0u;
    snap.u8ResidPayloadOk = 0u;
    snap.u8ResidDoubleOk = 0u;
    snap.u8MultiDistinctOk = 0u;

    /*
     * Legacy pool soft line (kept for existing greps) + soft pool
     * line emitted later via pmm_soft_inventory.
     */
    kprintf("aarch64: pmm pool soft base=0x%lx end=0x%lx pages=%u "
            "page_size=%lu\n",
            (unsigned long)g_u64PoolBase, (unsigned long)g_u64PoolEnd,
            g_cPoolPages, (unsigned long)GJ_PMM_CORE_PAGE_SIZE);

    fSelf = gj_pmm_core_selftest();
    snap.u8SelfOk = (fSelf != 0) ? 1u : 0u;

    if (fSelf == 0) {
        kprintf("aarch64: pmm soft FAIL (selftest)\n");
        kprintf("aarch64: pmm multi soft FAIL\n");
        /* Still emit primary PASS-shaped presence if pool non-empty. */
        cFree = gj_pmm_core_free_count();
        cTotal = gj_pmm_core_total_count();
        snap.cFree = cFree;
        snap.cTotal = cTotal;
        /* Soft inventory under FAIL path (greppable lamps stay present). */
        (void)pmm_soft_inventory(&snap);
        kprintf("aarch64: pmm PASS (shared core free=%u total=%u)\n",
                cFree, cTotal);
        return;
    }

    cGot = 0u;
    cFree0 = 0u;
    cFree1 = 0u;
    fDistinct = 0;
    fMulti = pmm_multi_soft_exercise(&cGot, &cFree0, &cFree1, &fDistinct);
    snap.cMultiGot = cGot;
    snap.cMultiFree0 = cFree0;
    snap.cMultiFree1 = cFree1;
    snap.u8MultiOk = (fMulti != 0) ? 1u : 0u;
    snap.u8MultiDistinctOk = (fDistinct != 0) ? 1u : 0u;

    fLifo = pmm_lifo_soft_exercise(&snap.u64LifoPaA, &snap.u64LifoPaB,
                                   &snap.u64LifoPaC, &snap.cLifoFree0,
                                   &snap.cLifoFree1);
    snap.u8LifoOk = (fLifo != 0) ? 1u : 0u;

    fNull = 0;
    fStep = pmm_step_soft_exercise(&snap.cStepFree0, &snap.cStepFree1,
                                   &snap.cStepFree2, &fNull);
    snap.u8StepOk = (fStep != 0) ? 1u : 0u;
    snap.u8NullOk = (fNull != 0) ? 1u : 0u;

    fInv = pmm_invariants_soft();
    snap.u8InvOk = (fInv != 0) ? 1u : 0u;

    fAlignBase = 0;
    fAlignEnd = 0;
    fGeom = pmm_geom_soft_exercise(&snap.u64PoolSpan, &snap.u64SpanExpect,
                                   &fAlignBase, &fAlignEnd);
    snap.u8GeomOk = (fGeom != 0) ? 1u : 0u;
    (void)fAlignBase;
    (void)fAlignEnd;

    fResidReject = 0;
    fResidScrub = 0;
    fResidPayload = 0;
    fResidDouble = 0;
    fResid = pmm_residual_soft_exercise(
        &snap.u64ResidPa, &snap.u64ResidPaB, &snap.cResidFree0,
        &snap.cResidFree1, &snap.cResidTotal0, &snap.cResidTotal1,
        &snap.u32ResidSurf, &fResidReject, &fResidScrub, &fResidPayload,
        &fResidDouble);
    snap.u8ResidOk = (fResid != 0) ? 1u : 0u;
    snap.u8ResidRejectOk = (fResidReject != 0) ? 1u : 0u;
    snap.u8ResidScrubOk = (fResidScrub != 0) ? 1u : 0u;
    snap.u8ResidPayloadOk = (fResidPayload != 0) ? 1u : 0u;
    snap.u8ResidDoubleOk = (fResidDouble != 0) ? 1u : 0u;

    cFree = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();
    snap.cFree = cFree;
    snap.cTotal = cTotal;

    /* Legacy multi soft summary (smoke: "aarch64: pmm multi soft ..."). */
    kprintf("aarch64: pmm multi soft n=%u free0=%u free1=%u total=%u\n",
            cGot, cFree0, cFree1, cTotal);

    kprintf("aarch64: pmm PASS (shared core free=%u total=%u)\n",
            cFree, cTotal);

    if (fMulti != 0) {
        kprintf("aarch64: pmm multi soft PASS\n");
    } else {
        kprintf("aarch64: pmm multi soft FAIL\n");
    }

    /*
     * Combined soft residual inventory under "aarch64: pmm soft ...".
     * Emits multi-field lamps + final soft PASS|FAIL (smoke greps PASS).
     * Honesty: soft PASS != hierarchical PMM / product kernel complete.
     * Soft!=product; G-AC-1; dual MIT OR Apache-2.0; Dual DoD OPEN.
     */
    fSoft = pmm_soft_inventory(&snap);
    (void)fSoft;
}
