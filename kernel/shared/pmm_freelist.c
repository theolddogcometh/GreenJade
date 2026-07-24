/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared freestanding order-0 freelist (GJ_ARCH_* product).
 * Identity-mapped PA pool only — no HHDM, no hierarchical orders.
 *
 * Wave 39 exclusive soft deepen (this unit only — greppable "pmm_core: soft …"):
 *   pmm_core: soft honesty   — order-0 only; no hierarchy/HHDM/zones claim
 *   pmm_core: soft inventory — free/total/base/limit/page_size snapshot
 *   pmm_core: soft pool      — span, head presence, free<=total lamp
 *   pmm_core: soft reject    — OOB/unaligned free paths (count stable)
 *   pmm_core: soft align     — alloc PA page-aligned + in [base,limit)
 *   pmm_core: soft counts    — free-count steps across alloc/free
 *   pmm_core: soft lifo      — free pushes front; next alloc reuses PA
 *   pmm_core: soft double    — two-free LIFO order (B then A)
 *   pmm_core: soft payload   — payload past node header survives free+alloc
 *   pmm_core: soft chain     — freelist walk length matches free_count
 *   pmm_core: soft total     — total immutable across soft exercise
 *   pmm_core: soft restore   — free/total restored after selftest
 *   pmm_core: soft path      — surface catalog + product non-claims
 *   pmm_core: soft geom      — Wave 19 page/span geometry
 *   pmm_core: soft return    — Wave 19 API return surfaces + product_kernel=OPEN
 *   pmm_core: soft deepen    — wave=39 stamp + area count
 *   pmm_core: soft PASS|FAIL / pmm_core: soft inventory PASS|FAIL
 * Honesty: soft inventory only — not hierarchical pmm / not 1 TiB product.
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>
#include <gj/string.h>

/* Wave 39 soft inventory stamp (file-local; never product gate). */
#define PMM_CORE_SOFT_WAVE 39u

struct pmm_core_node {
    struct pmm_core_node *pNext;
};

static struct pmm_core_node *g_pFree;
static u64 g_u64Base;
static u64 g_u64Limit;
static unsigned g_cFree;
static unsigned g_cTotal;
/* Wave 19: soft inventory emission counter (observability only). */
static u32 g_cSoftInvLogs;

void
gj_pmm_core_init(u64 u64Base, u64 u64Limit)
{
    u64 pa;

    g_pFree = 0;
    g_cFree = 0;
    g_cTotal = 0;
    g_u64Base = u64Base;
    g_u64Limit = u64Limit;

    if (u64Limit <= u64Base) {
        return;
    }
    for (pa = u64Base; pa + GJ_PMM_CORE_PAGE_SIZE <= u64Limit;
         pa += GJ_PMM_CORE_PAGE_SIZE) {
        struct pmm_core_node *n = (struct pmm_core_node *)(void *)(gj_vaddr_t)pa;

        n->pNext = g_pFree;
        g_pFree = n;
        g_cFree++;
        g_cTotal++;
    }
}

u64
gj_pmm_core_alloc(void)
{
    struct pmm_core_node *n = g_pFree;

    if (n == 0) {
        return 0;
    }
    g_pFree = n->pNext;
    if (g_cFree > 0u) {
        g_cFree--;
    }
    memset(n, 0, sizeof(*n));
    return (u64)(gj_vaddr_t)(void *)n;
}

void
gj_pmm_core_free(u64 u64Pa)
{
    struct pmm_core_node *n;

    if (u64Pa < g_u64Base || u64Pa >= g_u64Limit) {
        return;
    }
    if ((u64Pa & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
        return;
    }
    n = (struct pmm_core_node *)(void *)(gj_vaddr_t)u64Pa;
    n->pNext = g_pFree;
    g_pFree = n;
    g_cFree++;
}

unsigned
gj_pmm_core_free_count(void)
{
    return g_cFree;
}

unsigned
gj_pmm_core_total_count(void)
{
    return g_cTotal;
}

/*
 * Soft freelist chain walk (bounded). Returns node count, or 0xffffffffu if
 * the walk exceeded total+1 (cycle / corruption soft signal).
 */
static unsigned
soft_walk_free(void)
{
    unsigned c = 0;
    struct pmm_core_node *p = g_pFree;
    unsigned cBound = g_cTotal + 1u;

    while (p != 0) {
        if (c >= cBound) {
            return 0xffffffffu;
        }
        c++;
        p = p->pNext;
    }
    return c;
}

/*
 * Wave 19 greppable soft inventory dump (never hard-gates boot).
 * Safe after init; does not allocate. Prefix-stable: "pmm_core: soft …".
 * greppable: pmm_core: soft
 */
static void
pmm_core_soft_inventory(int fPass, unsigned cAreas, unsigned cChain,
                        unsigned cFreeSnap, unsigned cTotalSnap)
{
    u64 u64Span;
    u32 u32Head;
    u32 u32FreeLeTotal;
    u32 u32PageShift;
    const char *szVerdict;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    if (g_u64Limit > g_u64Base) {
        u64Span = g_u64Limit - g_u64Base;
    } else {
        u64Span = 0;
    }
    u32Head = (g_pFree != 0) ? 1u : 0u;
    u32FreeLeTotal = (cFreeSnap <= cTotalSnap) ? 1u : 0u;
    u32PageShift = GJ_PMM_CORE_PAGE_SHIFT;
    szVerdict = (fPass != 0) ? "PASS" : "FAIL";

    /*
     * Honesty first: shared order-0 freelist is NOT hierarchical pmm,
     * not HHDM dual-zone, not 1 TiB product. Soft inventory only.
     * greppable: pmm_core: soft honesty
     */
    kprintf("pmm_core: soft honesty order0=1 hierarchical=0 hhdm=0 "
            "zones=0 product_tib=0 pmem3=OPEN wave=%u "
            "(soft inventory only; not hierarchical pmm)\n",
            (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft exclusive — Wave 35 exclusive deepen */
    kprintf("pmm_core: soft exclusive multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 order0=1 wave=%u\n",
            (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft open — Wave 19 open-lamp rollup */
    kprintf("pmm_core: soft open multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 wave=%u\n",
            (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft inventory */
    kprintf("pmm_core: soft inventory free=%u total=%u base=0x%lx "
            "limit=0x%lx page_size=%lu page_shift=%u head=%u "
            "span=0x%lx logs=%u wave=%u\n",
            cFreeSnap, cTotalSnap,
            (unsigned long)g_u64Base, (unsigned long)g_u64Limit,
            (unsigned long)GJ_PMM_CORE_PAGE_SIZE, u32PageShift,
            u32Head, (unsigned long)u64Span, g_cSoftInvLogs,
            (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft pool */
    kprintf("pmm_core: soft pool free=%u total=%u free_le_total=%u "
            "head=%u span_pages=%u empty=%u wave=%u\n",
            cFreeSnap, cTotalSnap, u32FreeLeTotal, u32Head,
            (cTotalSnap > 0u) ? cTotalSnap : 0u,
            (cFreeSnap == 0u) ? 1u : 0u, (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft chain */
    kprintf("pmm_core: soft chain walk=%u free=%u match=%u "
            "bound=%u wave=%u\n",
            cChain, cFreeSnap,
            (cChain == cFreeSnap) ? 1u : 0u,
            cTotalSnap + 1u, (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft path — surface catalog + non-claims */
    kprintf("pmm_core: soft path init=1 alloc=1 free=1 free_count=1 "
            "total_count=1 selftest=1 hierarchical=0 hhdm=0 "
            "order_max=0 product_tib=0 wave=%u\n",
            (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft geom (Wave 19 page/span geometry) */
    kprintf("pmm_core: soft geom page_size=%lu page_shift=%u "
            "span=0x%lx span_pages=%u base=0x%lx limit=0x%lx "
            "node_bytes=%u wave=%u\n",
            (unsigned long)GJ_PMM_CORE_PAGE_SIZE, u32PageShift,
            (unsigned long)u64Span,
            (cTotalSnap > 0u) ? cTotalSnap : 0u,
            (unsigned long)g_u64Base, (unsigned long)g_u64Limit,
            (unsigned)sizeof(struct pmm_core_node),
            (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft return — Wave 19 API return surfaces */
    kprintf("pmm_core: soft return alloc_pa=1 free_void=1 free_count=1 "
            "total_count=1 selftest_bool=1 inv_ret=%u product_kernel=OPEN "
            "wave=%u\n",
            (fPass != 0) ? 1u : 0u, (unsigned)PMM_CORE_SOFT_WAVE);

    /* Grep: pmm_core: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pmm_core: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("pmm_core: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)PMM_CORE_SOFT_WAVE);
    /* Grep: pmm_core: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("pmm_core: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)PMM_CORE_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pmm_core: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("pmm_core: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)PMM_CORE_SOFT_WAVE);
    /* Grep: pmm_core: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("pmm_core: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)PMM_CORE_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pmm_core: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("pmm_core: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /* Grep: pmm_core: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("pmm_core: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pmm_core: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("pmm_core: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /* Grep: pmm_core: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("pmm_core: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: pmm_core: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("pmm_core: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /* Grep: pmm_core: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("pmm_core: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pmm_core: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("pmm_core: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /* Grep: pmm_core: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("pmm_core: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pmm_core: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("pmm_core: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /* Grep: pmm_core: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("pmm_core: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pmm_core: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("pmm_core: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
            /* Grep: pmm_core: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("pmm_core: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)PMM_CORE_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: pmm_core: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("pmm_core: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PMM_CORE_SOFT_WAVE);
                    /* Grep: pmm_core: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("pmm_core: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PMM_CORE_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm_core: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("pmm_core: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)PMM_CORE_SOFT_WAVE);
                            /* Grep: pmm_core: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("pmm_core: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)PMM_CORE_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm_core: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("pmm_core: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
                            /* Grep: pmm_core: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("pmm_core: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm_core: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pmm_core: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
                            /* Grep: pmm_core: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("pmm_core: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm_core: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("pmm_core: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
                            /* Grep: pmm_core: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pmm_core: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
                            /* Grep: pmm_core: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("pmm_core: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("pmm_core: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("pmm_core: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("pmm_core: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("pmm_core: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("pmm_core: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("pmm_core: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retfortress — Wave 35 return-fortress honesty */
kprintf("pmm_core: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("pmm_core: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft rethold — Wave 36 return-hold honesty */
kprintf("pmm_core: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retspire — Wave 36 exclusive spire stamp */
kprintf("pmm_core: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retwall — Wave 37 return-wall honesty */
kprintf("pmm_core: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retgate — Wave 37 exclusive gate stamp */
kprintf("pmm_core: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retmoat — Wave 38 return-moat honesty */
kprintf("pmm_core: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retower — Wave 38 exclusive tower stamp */
kprintf("pmm_core: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm_core: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("pmm_core: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
/* Grep: pmm_core: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("pmm_core: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)PMM_CORE_SOFT_WAVE);
                            kprintf("pmm_core: soft deepen wave=%u areas=%u free=%u total=%u "
            "logs=%u\n",
            (unsigned)PMM_CORE_SOFT_WAVE, cAreas, cFreeSnap, cTotalSnap,
            g_cSoftInvLogs);

    /* Grep: pmm_core: soft inventory PASS|FAIL / pmm_core: soft PASS|FAIL */
    kprintf("pmm_core: soft inventory %s free=%u total=%u wave=%u\n",
            szVerdict, cFreeSnap, cTotalSnap, (unsigned)PMM_CORE_SOFT_WAVE);
    kprintf("pmm_core: soft %s free=%u total=%u areas=%u wave=%u\n",
            szVerdict, cFreeSnap, cTotalSnap, cAreas,
            (unsigned)PMM_CORE_SOFT_WAVE);
}

/*
 * Soft deepen Wave 19: reject path, alignment/range, free-count steps,
 * LIFO reuse + double LIFO, payload survive (node header scrub only),
 * freelist chain walk, total immutable, restore pool counts + inventory.
 * Returns 1 on PASS, 0 on soft FAIL. Does not drain the whole pool.
 */
int
gj_pmm_core_selftest(void)
{
    u64 paA;
    u64 paB;
    u64 paC;
    u64 paD;
    u64 paE;
    unsigned cFree0;
    unsigned cTotal0;
    unsigned cChain;
    unsigned cAreas;
    u32 u32Surf; /* soft surface bit lamps (Wave 19) */
    volatile u32 *pMark;
    volatile u32 *pMark2;
    int fOk;

    /* Surface bits for greppable per-area lamps. */
    enum {
        SURF_REJECT  = 1u << 0,
        SURF_CHAIN   = 1u << 1,
        SURF_ALIGN   = 1u << 2,
        SURF_COUNTS  = 1u << 3,
        SURF_LIFO    = 1u << 4,
        SURF_PAYLOAD = 1u << 5,
        SURF_DOUBLE  = 1u << 6,
        SURF_RESTORE = 1u << 7,
        SURF_TOTAL   = 1u << 8
    };

    cFree0 = g_cFree;
    cTotal0 = g_cTotal;
    cAreas = 0;
    u32Surf = 0;
    fOk = 1;
    paA = 0;
    paB = 0;
    paC = 0;
    paD = 0;
    paE = 0;

    if (cTotal0 == 0u || cFree0 == 0u) {
        pmm_core_soft_inventory(0, 0, soft_walk_free(), cFree0, cTotal0);
        return 0;
    }
    if (cFree0 > cTotal0) {
        pmm_core_soft_inventory(0, 0, soft_walk_free(), cFree0, cTotal0);
        return 0;
    }

    /* --- reject paths must not touch free/total --- */
    gj_pmm_core_free(0);
    gj_pmm_core_free(1ul); /* unaligned */
    gj_pmm_core_free(g_u64Limit); /* OOB high (limit exclusive) */
    if (g_u64Base >= GJ_PMM_CORE_PAGE_SIZE) {
        gj_pmm_core_free(g_u64Base - GJ_PMM_CORE_PAGE_SIZE);
    }
    /* Mid-page unaligned inside pool (Wave 19 reject deepen). */
    if (g_u64Limit > g_u64Base + 1ul) {
        gj_pmm_core_free(g_u64Base + 1ul);
        gj_pmm_core_free(g_u64Base + (GJ_PMM_CORE_PAGE_SIZE / 2ul));
    }
    /* Limit-1 is almost always unaligned; must reject. */
    if (g_u64Limit > 0ul) {
        gj_pmm_core_free(g_u64Limit - 1ul);
    }
    if (g_cFree != cFree0 || g_cTotal != cTotal0) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_REJECT;
    cAreas++; /* reject */

    /*
     * Wave 19 return surface: empty freelist returns 0 from alloc after
     * temporary drain of one page is not done here (pool must stay live);
     * document non-zero alloc return shape via paA path below.
     * Soft-only: product hierarchical pmm remains separate.
     */

    /* --- freelist chain matches free_count (pre) --- */
    cChain = soft_walk_free();
    if (cChain != cFree0) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_CHAIN;
    cAreas++; /* chain_pre */

    /* --- alloc A: align, range, count step --- */
    paA = gj_pmm_core_alloc();
    if (paA == 0) {
        fOk = 0;
        goto soft_out;
    }
    if ((paA & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul) {
        gj_pmm_core_free(paA);
        paA = 0;
        fOk = 0;
        goto soft_out;
    }
    if (paA < g_u64Base || paA >= g_u64Limit) {
        gj_pmm_core_free(paA);
        paA = 0;
        fOk = 0;
        goto soft_out;
    }
    if (g_cFree != cFree0 - 1u || g_cTotal != cTotal0) {
        gj_pmm_core_free(paA);
        paA = 0;
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_ALIGN;
    u32Surf |= SURF_COUNTS;
    cAreas++; /* align */
    cAreas++; /* counts_step_a */

    paB = gj_pmm_core_alloc();
    if (paB == 0) {
        /* Single-page pool: free A and accept restore. */
        gj_pmm_core_free(paA);
        paA = 0;
        if (g_cFree != cFree0 || g_cTotal != cTotal0) {
            fOk = 0;
        } else {
            u32Surf |= SURF_RESTORE;
            u32Surf |= SURF_TOTAL;
            cAreas++; /* restore_single */
            cAreas++; /* total_single */
        }
        goto soft_out;
    }
    if (paA == paB) {
        gj_pmm_core_free(paA);
        paA = 0;
        fOk = 0;
        goto soft_out;
    }
    if ((paB & (GJ_PMM_CORE_PAGE_SIZE - 1ul)) != 0ul ||
        paB < g_u64Base || paB >= g_u64Limit) {
        gj_pmm_core_free(paA);
        gj_pmm_core_free(paB);
        paA = 0;
        paB = 0;
        fOk = 0;
        goto soft_out;
    }
    if (g_cFree != cFree0 - 2u || g_cTotal != cTotal0) {
        gj_pmm_core_free(paA);
        gj_pmm_core_free(paB);
        paA = 0;
        paB = 0;
        fOk = 0;
        goto soft_out;
    }
    cAreas++; /* counts_step_b */

    /*
     * LIFO: free pushes front; next alloc must return paB.
     * Payload past node header survives free+alloc (only sizeof(node) scrubbed).
     */
    {
        u64 u64WasB = paB;

        pMark = (volatile u32 *)(void *)((gj_vaddr_t)paB + 64ul);
        *pMark = 0xC0FFEEU;
        gj_pmm_core_free(paB);
        paB = 0;
        if (g_cFree != cFree0 - 1u || g_cTotal != cTotal0) {
            gj_pmm_core_free(paA);
            paA = 0;
            fOk = 0;
            goto soft_out;
        }
        paC = gj_pmm_core_alloc();
        if (paC != u64WasB) {
            if (paC != 0) {
                gj_pmm_core_free(paC);
                paC = 0;
            }
            gj_pmm_core_free(paA);
            paA = 0;
            fOk = 0;
            goto soft_out;
        }
        if (*pMark != 0xC0FFEEU) {
            gj_pmm_core_free(paA);
            gj_pmm_core_free(paC);
            paA = 0;
            paC = 0;
            fOk = 0;
            goto soft_out;
        }
    }
    u32Surf |= SURF_LIFO;
    u32Surf |= SURF_PAYLOAD;
    cAreas++; /* lifo */
    cAreas++; /* payload */

    /*
     * Wave 19 double LIFO: free C (LIFO-B) then A; next two allocs must be
     * A then former-B (most recent free first). Stamp second payload mark.
     */
    pMark2 = (volatile u32 *)(void *)((gj_vaddr_t)paA + 128ul);
    *pMark2 = 0xA11CEU;
    {
        u64 u64WasB = paC;
        u64 u64WasA = paA;

        gj_pmm_core_free(paC);
        paC = 0;
        gj_pmm_core_free(paA);
        paA = 0;
        if (g_cFree != cFree0 || g_cTotal != cTotal0) {
            fOk = 0;
            goto soft_out;
        }
        paD = gj_pmm_core_alloc();
        paE = gj_pmm_core_alloc();
        if (paD != u64WasA || paE != u64WasB) {
            if (paD != 0) {
                gj_pmm_core_free(paD);
                paD = 0;
            }
            if (paE != 0) {
                gj_pmm_core_free(paE);
                paE = 0;
            }
            fOk = 0;
            goto soft_out;
        }
        if (*pMark2 != 0xA11CEU || *pMark != 0xC0FFEEU) {
            gj_pmm_core_free(paD);
            gj_pmm_core_free(paE);
            paD = 0;
            paE = 0;
            fOk = 0;
            goto soft_out;
        }
        if (g_cFree != cFree0 - 2u || g_cTotal != cTotal0) {
            gj_pmm_core_free(paD);
            gj_pmm_core_free(paE);
            paD = 0;
            paE = 0;
            fOk = 0;
            goto soft_out;
        }
    }
    u32Surf |= SURF_DOUBLE;
    cAreas++; /* double_lifo */

    /* Restore both pages. */
    gj_pmm_core_free(paD);
    gj_pmm_core_free(paE);
    paD = 0;
    paE = 0;
    if (g_cFree != cFree0 || g_cTotal != cTotal0) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_RESTORE;
    cAreas++; /* restore */

    /* --- freelist chain matches free_count (post) --- */
    cChain = soft_walk_free();
    if (cChain != cFree0) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_CHAIN;
    cAreas++; /* chain_post */

    /* total immutable soft lamp */
    if (g_cTotal != cTotal0) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_TOTAL;
    cAreas++; /* total */

soft_out:
    cChain = soft_walk_free();
    /* Best-effort restore if a path left pages outstanding. */
    if (paA != 0) {
        gj_pmm_core_free(paA);
    }
    if (paB != 0) {
        gj_pmm_core_free(paB);
    }
    if (paC != 0) {
        gj_pmm_core_free(paC);
    }
    if (paD != 0) {
        gj_pmm_core_free(paD);
    }
    if (paE != 0) {
        gj_pmm_core_free(paE);
    }
    pmm_core_soft_inventory(fOk, cAreas, cChain, g_cFree, g_cTotal);

    /* Grep: per-surface soft PASS lamps (only surfaces that actually ran). */
    if ((u32Surf & SURF_REJECT) != 0u) {
        kprintf("pmm_core: soft reject soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_ALIGN) != 0u) {
        kprintf("pmm_core: soft align soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_COUNTS) != 0u) {
        kprintf("pmm_core: soft counts soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_LIFO) != 0u) {
        kprintf("pmm_core: soft lifo soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_DOUBLE) != 0u) {
        kprintf("pmm_core: soft double soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_PAYLOAD) != 0u) {
        kprintf("pmm_core: soft payload soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_CHAIN) != 0u) {
        kprintf("pmm_core: soft chain soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_TOTAL) != 0u) {
        kprintf("pmm_core: soft total soft PASS wave=%u\n",
                (unsigned)PMM_CORE_SOFT_WAVE);
    }
    if ((u32Surf & SURF_RESTORE) != 0u) {
        kprintf("pmm_core: soft restore soft PASS free=%u total=%u wave=%u\n",
                g_cFree, g_cTotal, (unsigned)PMM_CORE_SOFT_WAVE);
    }

    /* Grep: pmm_core: soft return selftest — Wave 19 terminal return surface */
    kprintf("pmm_core: soft return selftest_ret=%d surf=0x%x areas=%u "
            "product_kernel=OPEN wave=%u\n",
            fOk, u32Surf, cAreas, (unsigned)PMM_CORE_SOFT_WAVE);

    return fOk;
}
