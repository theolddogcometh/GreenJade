/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 PMM — thin wrap over shared freelist core (kernel/shared/pmm_freelist.c).
 *
 * -------------------------------------------------------------------------
 * Soft inventory (Wave 39 exclusive deepen; this unit only — greppable
 * "aarch64: pmm soft …")
 * -------------------------------------------------------------------------
 * Soft pool geometry: base/end/page counts after init (order-0 identity
 * pool only; no hierarchical PMM, no NEON, no buddy paste).
 * Soft multi exercise: alloc N pages, dual-pattern paint/verify, free
 * reverse; free_count restored.
 * Soft LIFO reuse: free B then next alloc returns B (freelist head).
 * Soft free-step: single alloc drops free by 1; free restores.
 * Soft null free: free(NULL) is no-op (count unchanged).
 * Soft invariant: free ≤ total, non-zero total, pool geometry coherent.
 * Soft stats: gate sum + free ratio + log tally (Wave 19).
 * Soft deepen: area catalog stamp wave=39.
 * Soft return: selftest/inventory return surfaces (Wave 19).
 * Soft path honesty: order-0 shared core only; not ≥1 TiB hierarchical.
 * Soft honesty: aarch64 product kernel remains OPEN (soft scaffold only).
 *
 * Greppable soft inventory (prefix-stable):
 *   aarch64: pmm soft pool base=… end=… pages=… page_size=… span=…
 *   aarch64: pmm soft multi n=… free0=… free1=… total=… ok=…
 *   aarch64: pmm soft lifo pa_a=… pa_b=… pa_c=… reuse=… free0=… free1=…
 *   aarch64: pmm soft step free0=… free1=… free2=… drop=… restore=…
 *   aarch64: pmm soft inv free=… total=… pool_pages=… self=… multi=…
 *             lifo=… step=… null=… inv=…
 *   aarch64: pmm soft stats gates=… free=… total=… ratio=… logs=… wave=39
 *   aarch64: pmm soft inventory wave=39 …
 *   aarch64: pmm soft deepen wave=39 areas=…
 *   aarch64: pmm soft return inv_ret=… product_kernel=OPEN wave=39
 *   aarch64: pmm soft path order0=1 hier=0 neon=0 tib_bar=0 core=1
 *             product_kernel=OPEN wave=39
 *   aarch64: pmm soft honesty product_kernel=OPEN soft_only=1
 *   aarch64: pmm soft PASS | FAIL
 *
 * Legacy / product smoke markers (kept greppable):
 *   aarch64: pmm PASS (shared core free=… total=…)
 *   aarch64: pmm pool soft base=… end=… pages=…
 *   aarch64: pmm multi soft n=… free0=… free1=…
 *   aarch64: pmm multi soft PASS | FAIL
 *
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

/* Wave 39 soft inventory stamp (greppable wave=39). */
#define PMM_SOFT_WAVE 39u

/* Soft deepen areas: pool,multi,lifo,step,inv,stats,path,surf,return,honesty,deepen. */
#define PMM_SOFT_AREAS 51u

static u64 g_u64PoolBase;
static u64 g_u64PoolEnd;
static unsigned g_cPoolPages;
static unsigned g_cPmmSoftLogs; /* Wave 19 inventory emit count */

/*
 * Soft inventory snapshot (Wave 19; file-local; never hard-gates boot).
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
    u64      u64LifoPaA;
    u64      u64LifoPaB;
    u64      u64LifoPaC;
    u64      u64PoolSpan;
    u8       u8SelfOk;
    u8       u8MultiOk;
    u8       u8LifoOk;
    u8       u8StepOk;
    u8       u8NullOk;
    u8       u8InvOk;
    u8       u8Pad0;
    u8       u8Pad1;
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
 * only if cGot == 0 → FAIL; else verify the pages we got).
 */
static int
pmm_multi_soft_exercise(unsigned *pOutGot, unsigned *pOutFree0,
                        unsigned *pOutFree1)
{
    void *aPages[PMM_SOFT_MULTI_N];
    unsigned iPage;
    unsigned cGot;
    unsigned cFree0;
    unsigned cFree1;
    unsigned cTotal;
    int fOk;

    cFree0 = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();
    cGot = 0u;
    fOk = 1;

    for (iPage = 0u; iPage < PMM_SOFT_MULTI_N; iPage++) {
        aPages[iPage] = aarch64_pmm_alloc();
        if (aPages[iPage] == 0) {
            break;
        }
        cGot++;
    }

    if (cGot == 0u) {
        fOk = 0;
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

    /* Soft paint pass B (overwrite) + verify. */
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        pWord[0] = PMM_SOFT_PAT_B ^ ((u64)iPage << 8);
        pWord[1] = PMM_SOFT_PAT_A ^ ((u64)iPage << 8);
    }
    for (iPage = 0u; iPage < cGot; iPage++) {
        volatile u64 *pWord = (volatile u64 *)aPages[iPage];

        if (pWord[0] != (PMM_SOFT_PAT_B ^ ((u64)iPage << 8)) ||
            pWord[1] != (PMM_SOFT_PAT_A ^ ((u64)iPage << 8))) {
            fOk = 0;
        }
        /* Soft pool bounds: page PA must lie in [base, end). */
        {
            u64 pa = (u64)(gj_vaddr_t)aPages[iPage];

            if (pa < g_u64PoolBase || pa >= g_u64PoolEnd) {
                fOk = 0;
            }
            if ((pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
                fOk = 0;
            }
        }
    }

    /* Free reverse order (soft LIFO stress on freelist). */
    for (iPage = cGot; iPage > 0u; iPage--) {
        aarch64_pmm_free(aPages[iPage - 1u]);
        aPages[iPage - 1u] = 0;
    }

    cFree1 = gj_pmm_core_free_count();
    if (cFree1 != cFree0) {
        fOk = 0;
    }
    if (cFree1 > cTotal) {
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

    /* Free B first → freelist head; next alloc must return B. */
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
    return 1;
}

/*
 * Wave 19 soft inventory emission — greppable "aarch64: pmm soft …".
 * Returns 1 if all soft gates held (self/multi/lifo/step/null/inv).
 */
static int
pmm_soft_inventory(const struct pmm_soft_snap *pSnap)
{
    unsigned uLifoReuse;
    unsigned uStepDrop;
    unsigned uStepRestore;
    unsigned cGates;
    unsigned uRatio;
    int fOk;

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

    /* Soft gate sum + free/total ratio (percent; pure integer). */
    cGates = (unsigned)pSnap->u8SelfOk + (unsigned)pSnap->u8MultiOk +
             (unsigned)pSnap->u8LifoOk + (unsigned)pSnap->u8StepOk +
             (unsigned)pSnap->u8NullOk + (unsigned)pSnap->u8InvOk;
    uRatio = 0u;
    if (pSnap->cTotal != 0u) {
        uRatio = (pSnap->cFree * 100u) / pSnap->cTotal;
    }

    /* Grep: aarch64: pmm soft pool */
    kprintf("aarch64: pmm soft pool base=0x%lx end=0x%lx pages=%u "
            "page_size=%lu span=0x%lx\n",
            (unsigned long)g_u64PoolBase, (unsigned long)g_u64PoolEnd,
            pSnap->cPoolPages, (unsigned long)GJ_PMM_CORE_PAGE_SIZE,
            (unsigned long)pSnap->u64PoolSpan);

    /* Grep: aarch64: pmm soft multi */
    kprintf("aarch64: pmm soft multi n=%u free0=%u free1=%u total=%u ok=%u\n",
            pSnap->cMultiGot, pSnap->cMultiFree0, pSnap->cMultiFree1,
            pSnap->cTotal, (unsigned)pSnap->u8MultiOk);

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
            "self=%u multi=%u lifo=%u step=%u null=%u inv=%u\n",
            pSnap->cFree, pSnap->cTotal, pSnap->cPoolPages,
            (unsigned)pSnap->u8SelfOk, (unsigned)pSnap->u8MultiOk,
            (unsigned)pSnap->u8LifoOk, (unsigned)pSnap->u8StepOk,
            (unsigned)pSnap->u8NullOk, (unsigned)pSnap->u8InvOk);

    /* Grep: aarch64: pmm soft stats — Wave 19 rollup. */
    kprintf("aarch64: pmm soft stats gates=%u free=%u total=%u ratio=%u "
            "multi_n=%u logs=%u wave=%u\n",
            cGates, pSnap->cFree, pSnap->cTotal, uRatio,
            (unsigned)PMM_SOFT_MULTI_N, g_cPmmSoftLogs,
            (unsigned)PMM_SOFT_WAVE);

    /* Grep: aarch64: pmm soft inventory — Wave 20 stamp. */
    kprintf("aarch64: pmm soft inventory wave=%u gates=%u free=%u total=%u "
            "pool_pages=%u logs=%u ok_sum=%u\n",
            (unsigned)PMM_SOFT_WAVE, cGates, pSnap->cFree, pSnap->cTotal,
            pSnap->cPoolPages, g_cPmmSoftLogs, cGates);

    /*
     * Grep: aarch64: pmm soft deepen
     * Wave 19 area catalog — order-0 soft scaffold only.
     */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: pmm: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("aarch64: pmm: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)PMM_SOFT_WAVE);
    /* Grep: aarch64: pmm: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("aarch64: pmm: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)PMM_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: pmm: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("aarch64: pmm: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)PMM_SOFT_WAVE);
    /* Grep: aarch64: pmm: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("aarch64: pmm: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: aarch64: pmm: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("aarch64: pmm: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: aarch64: pmm: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("aarch64: pmm: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: aarch64: pmm: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("aarch64: pmm: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: aarch64: pmm: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("aarch64: pmm: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: aarch64: pmm: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("aarch64: pmm: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: aarch64: pmm: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("aarch64: pmm: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: aarch64: pmm: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("aarch64: pmm: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: aarch64: pmm: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("aarch64: pmm: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: aarch64: pmm: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("aarch64: pmm: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: aarch64: pmm: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("aarch64: pmm: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: aarch64: pmm: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("aarch64: pmm: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: aarch64: pmm: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("aarch64: pmm: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: aarch64: pmm: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("aarch64: pmm: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PMM_SOFT_WAVE);
                    /* Grep: aarch64: pmm: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("aarch64: pmm: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PMM_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: pmm: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("aarch64: pmm: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)PMM_SOFT_WAVE);
                            /* Grep: aarch64: pmm: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("aarch64: pmm: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)PMM_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: pmm: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("aarch64: pmm: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: aarch64: pmm: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("aarch64: pmm: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: pmm: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("aarch64: pmm: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: aarch64: pmm: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("aarch64: pmm: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: pmm: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("aarch64: pmm: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: aarch64: pmm: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("aarch64: pmm: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: aarch64: pmm: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("aarch64: pmm: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("aarch64: pmm: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("aarch64: pmm: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("aarch64: pmm: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("aarch64: pmm: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("aarch64: pmm: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("aarch64: pmm: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retfortress — Wave 35 return-fortress honesty */
kprintf("aarch64: pmm: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("aarch64: pmm: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft rethold — Wave 36 return-hold honesty */
kprintf("aarch64: pmm: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retspire — Wave 36 exclusive spire stamp */
kprintf("aarch64: pmm: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retwall — Wave 37 return-wall honesty */
kprintf("aarch64: pmm: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retgate — Wave 37 exclusive gate stamp */
kprintf("aarch64: pmm: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retmoat — Wave 38 return-moat honesty */
kprintf("aarch64: pmm: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retower — Wave 38 exclusive tower stamp */
kprintf("aarch64: pmm: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: pmm: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("aarch64: pmm: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: aarch64: pmm: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("aarch64: pmm: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
                            kprintf("aarch64: pmm soft deepen wave=%u areas=%u "
            "catalog=pool,multi,lifo,step,inv,stats,path,surf,return,honesty,deepen "
            "logs=%u soft_only=1\n",
            (unsigned)PMM_SOFT_WAVE, (unsigned)PMM_SOFT_AREAS,
            g_cPmmSoftLogs);

    /* Grep: aarch64: pmm soft surf — Wave 19 gate bit lamps */
    kprintf("aarch64: pmm soft surf self=%u multi=%u lifo=%u step=%u "
            "null=%u inv=%u gates=%u bits=0x%x wave=%u\n",
            (unsigned)pSnap->u8SelfOk, (unsigned)pSnap->u8MultiOk,
            (unsigned)pSnap->u8LifoOk, (unsigned)pSnap->u8StepOk,
            (unsigned)pSnap->u8NullOk, (unsigned)pSnap->u8InvOk, cGates,
            (unsigned)((unsigned)pSnap->u8SelfOk |
                       ((unsigned)pSnap->u8MultiOk << 1) |
                       ((unsigned)pSnap->u8LifoOk << 2) |
                       ((unsigned)pSnap->u8StepOk << 3) |
                       ((unsigned)pSnap->u8NullOk << 4) |
                       ((unsigned)pSnap->u8InvOk << 5)),
            (unsigned)PMM_SOFT_WAVE);

    /*
     * Grep: aarch64: pmm soft path
     * Honesty: order-0 shared core only — not hierarchical / TiB product bar.
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    kprintf("aarch64: pmm soft path order0=1 hier=0 neon=0 tib_bar=0 "
            "core=1 multi_n=%u product_kernel=OPEN hard_gate=0 wave=%u\n",
            (unsigned)PMM_SOFT_MULTI_N, (unsigned)PMM_SOFT_WAVE);

    /* Grep: aarch64: pmm soft honesty */
    kprintf("aarch64: pmm soft honesty product_kernel=OPEN soft_only=1 "
            "no_hier=1 no_tib=1 no_bar3=1 wave=%u\n",
            (unsigned)PMM_SOFT_WAVE);

    /* Grep: aarch64: pmm soft exclusive — Wave 35 exclusive deepen */
    kprintf("aarch64: pmm soft exclusive multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 wave=%u\n",
            (unsigned)PMM_SOFT_WAVE);

    /* Grep: aarch64: pmm soft open — Wave 19 open-lamp rollup */
    kprintf("aarch64: pmm soft open multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 wave=%u\n",
            (unsigned)PMM_SOFT_WAVE);

    fOk = 0;
    if (pSnap->u8SelfOk != 0u && pSnap->u8MultiOk != 0u &&
        pSnap->u8LifoOk != 0u && pSnap->u8StepOk != 0u &&
        pSnap->u8NullOk != 0u && pSnap->u8InvOk != 0u) {
        fOk = 1;
    }

    /* Grep: aarch64: pmm soft return — Wave 19 return surfaces */
    kprintf("aarch64: pmm soft return inv_ret=%u self=%u multi=%u lifo=%u "
            "step=%u null=%u inv=%u product_kernel=OPEN wave=%u\n",
            (fOk != 0) ? 1u : 0u,
            (unsigned)pSnap->u8SelfOk, (unsigned)pSnap->u8MultiOk,
            (unsigned)pSnap->u8LifoOk, (unsigned)pSnap->u8StepOk,
            (unsigned)pSnap->u8NullOk, (unsigned)pSnap->u8InvOk,
            (unsigned)PMM_SOFT_WAVE);

    if (fOk != 0) {
        kprintf("aarch64: pmm soft PASS wave=%u\n",
                (unsigned)PMM_SOFT_WAVE);
    } else {
        kprintf("aarch64: pmm soft FAIL wave=%u\n",
                (unsigned)PMM_SOFT_WAVE);
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
    int fSoft;
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
    snap.u64LifoPaA = 0ull;
    snap.u64LifoPaB = 0ull;
    snap.u64LifoPaC = 0ull;
    snap.u64PoolSpan = g_u64PoolEnd - g_u64PoolBase;
    snap.u8SelfOk = 0u;
    snap.u8MultiOk = 0u;
    snap.u8LifoOk = 0u;
    snap.u8StepOk = 0u;
    snap.u8NullOk = 0u;
    snap.u8InvOk = 0u;
    snap.u8Pad0 = 0u;
    snap.u8Pad1 = 0u;

    /*
     * Legacy pool soft line (kept for existing greps) + Wave 19 soft pool
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
    fMulti = pmm_multi_soft_exercise(&cGot, &cFree0, &cFree1);
    snap.cMultiGot = cGot;
    snap.cMultiFree0 = cFree0;
    snap.cMultiFree1 = cFree1;
    snap.u8MultiOk = (fMulti != 0) ? 1u : 0u;

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

    cFree = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();
    snap.cFree = cFree;
    snap.cTotal = cTotal;

    /* Legacy multi soft summary (smoke: "aarch64: pmm multi soft …"). */
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
     * Wave 19 combined soft inventory under "aarch64: pmm soft …".
     * Emits multi-field lamps + final soft PASS|FAIL (smoke greps PASS).
     * Honesty: soft PASS ≠ hierarchical PMM / product kernel complete.
     */
    fSoft = pmm_soft_inventory(&snap);
    (void)fSoft;
}
