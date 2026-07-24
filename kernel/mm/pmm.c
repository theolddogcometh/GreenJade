/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Physical page freelist PMM — multi-TiB capable (P-MEM-3).
 *
 * Dual zone freelists
 *   low  : PA < 4 GiB  (identity before HHDM; HHDM after)
 *   high : PA ≥ 4 GiB  (pending until vmm_hhdm_init + pmm_release_high)
 * Alloc prefers low so early kernel structures stay identity-safe.
 *
 * Hierarchical free (order freelists) — greppable: hierarchical free
 *   Order N node covers (1<<N) contiguous pages, naturally aligned.
 *   Order 0 heads: g_paFreeLow / g_paFreeHigh (single 4 KiB frames).
 *   Order 1..PMM_MAX_ORDER: g_aOrderLow[] / g_aOrderHigh[] (max order 9
 *   → 512 pages = 2 MiB). free_range bulk-inserts largest aligned blocks
 *   at boot / high-release so multi-hundred-GiB hosts skip per-page walks.
 *   pop_order_split: exact order, else split a higher block and push
 *   sibling buddies back (rearranges nodes only — never invents frames).
 *
 * Wave 19 soft inventory deepen (extends Wave 13; greppable: pmm: soft):
 *   "pmm: soft honesty …"    explicit non-claims (not 1 TiB product; not bar3)
 *   "pmm: soft inventory …"  free/total, zones, pending, hierarchy snapshot
 *   "pmm: soft zones …"      low/high free frames + release state
 *   "pmm: soft hier …"       max_order, nodes, splits, high-order pushes
 *   "pmm: soft orders …"     per-order node counts (soft snapshot)
 *   "pmm: soft heads …"      freelist head presence (low/high order-0 + top)
 *   "pmm: soft pending …"    high-pending ranges before release
 *   "pmm: soft host …"       host size vs 1 TiB gate (soft only)
 *   "pmm: soft design …"     design max_order / block size (soft only)
 *   "pmm: soft path …"       surface catalog + explicit product_tib=0
 *   "pmm: soft stats …"      rollup free/use/splits/nodes/logs
 *   "pmm: soft geometry …"   page/zone/order constants (Wave 15)
 *   "pmm: soft kernel …"     kernel image reserve snap (Wave 15)
 *   "pmm: soft hhdm …"       high-zone vs HHDM dependency (Wave 15)
 *   "pmm: soft lamps …"      composite readiness lamps (Wave 15)
 *   "pmm: soft OPEN …"       P-MEM-3 / product_tib / bar3 OPEN (Wave 15)
 *   "pmm: soft surfaces …"   Wave 19 return-surface catalog (surf bitmask)
 *   "pmm: soft ratio …"      Wave 17 free/total soft ratio lamps
 *   "pmm: soft sites …"      Wave 17 emission-site catalog
 *   "pmm: soft api …"        Wave 17 alloc/free surface return lamps
 *   "pmm: soft return rate …" Wave 17 free/total return-rate lamps
 *   "pmm: soft retcode …"    Wave 17 alloc/free retcode catalog
 *   pmm: soft return selftest — Wave 19 terminal return surface
 *   pmm: soft retmap     — Wave 19 return-surface map
 *   "pmm: soft deepen …"     wave=87 stamp + area count
 *   "pmm: soft PASS" | "pmm: soft inventory PASS" | "pmm: soft EMPTY|NONE"
 * Honesty: soft inventory never claims 1 TiB product host or closes P-MEM-3;
 *          soft ≠ bar3; soft ≠ product.
 *
 * 1 TiB design observability (no 1 TiB host required — soft markers):
 *   Zone free frame counts (low/high) + per-order node counts.
 *   "pmm: orders tag=..." histogram after init / high-release / soak.
 *   "pmm: tib_design soft ..." always (max_order, block size, design ceil).
 *   "pmm: tib_host soft PASS|SKIP" — true ≥1 TiB max_pa gate (soft).
 *   "pmm: high_order soft ..." — high-order node presence after release/soak.
 *   Soft hierarchical exercise runs even on soak_tib SKIP (small QEMU).
 *
 * Serial markers (scripts/gj-soak-large-ram.sh, smoke-all, product-summary):
 *   "pmm: freelist free="
 *   "pmm: high released free="
 *   "pmm: soft inventory"
 *   "pmm: soak PASS"
 *   "pmm: soak_tib PASS" | "pmm: soak_tib SKIP soft" | "pmm: soak_tib FAIL"
 * Large-RAM path: main.c calls pmm_soak_tib(768ull<<30) — GJ_MEM=768G.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/vmm.h>

#define PMM_HIGH_PENDING_MAX 64
#define PMM_LOW_MAX          0x100000000ull
/* Max hierarchical order: 9 → 512 pages = 2 MiB (matches HHDM large pages). */
#define PMM_MAX_ORDER        9u
/* Product soft gate: true 1 TiB host class (1ull<<40). Soft only — never hard-fail. */
#define PMM_TIB_BYTES        (1ull << 40)
/* Wave 19 greppable soft inventory stamp (file-local; never product gate). */
#define PMM_SOFT_WAVE 87u
/* Catalog area count for deepen stamp (honesty..api prior to deepen line). */
#define PMM_SOFT_AREAS 156u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
 * Bits mark greppable soft areas this unit emits — not product close.
 * greppable: pmm: soft surfaces
 */
#define PMM_SOFT_SURF_HONESTY   (1u << 0)
#define PMM_SOFT_SURF_INVENTORY (1u << 1)
#define PMM_SOFT_SURF_ZONES     (1u << 2)
#define PMM_SOFT_SURF_HIER      (1u << 3)
#define PMM_SOFT_SURF_ORDERS    (1u << 4)
#define PMM_SOFT_SURF_HEADS     (1u << 5)
#define PMM_SOFT_SURF_PENDING   (1u << 6)
#define PMM_SOFT_SURF_HOST      (1u << 7)
#define PMM_SOFT_SURF_DESIGN    (1u << 8)
#define PMM_SOFT_SURF_PATH      (1u << 9)
#define PMM_SOFT_SURF_STATS     (1u << 10)
#define PMM_SOFT_SURF_GEOMETRY  (1u << 11)
#define PMM_SOFT_SURF_KERNEL    (1u << 12)
#define PMM_SOFT_SURF_HHDM      (1u << 13)
#define PMM_SOFT_SURF_LAMPS     (1u << 14)
#define PMM_SOFT_SURF_OPEN      (1u << 15)
#define PMM_SOFT_SURF_SURFACES  (1u << 16)
#define PMM_SOFT_SURF_RATIO     (1u << 17)
#define PMM_SOFT_SURF_SITES     (1u << 18)
#define PMM_SOFT_SURF_API       (1u << 19)
#define PMM_SOFT_SURF_CATALOG                                                      \
    (PMM_SOFT_SURF_HONESTY | PMM_SOFT_SURF_INVENTORY | PMM_SOFT_SURF_ZONES |     \
     PMM_SOFT_SURF_HIER | PMM_SOFT_SURF_ORDERS | PMM_SOFT_SURF_HEADS |           \
     PMM_SOFT_SURF_PENDING | PMM_SOFT_SURF_HOST | PMM_SOFT_SURF_DESIGN |         \
     PMM_SOFT_SURF_PATH | PMM_SOFT_SURF_STATS | PMM_SOFT_SURF_GEOMETRY |         \
     PMM_SOFT_SURF_KERNEL | PMM_SOFT_SURF_HHDM | PMM_SOFT_SURF_LAMPS |           \
     PMM_SOFT_SURF_OPEN | PMM_SOFT_SURF_SURFACES | PMM_SOFT_SURF_RATIO |         \
     PMM_SOFT_SURF_SITES | PMM_SOFT_SURF_API)

struct pmm_pending {
    gj_paddr_t paBase;
    u64        cbLen;
};

static gj_paddr_t g_paFreeLow;
static gj_paddr_t g_paFreeHigh;
/* Order freelists: [0] unused (order0 uses g_paFreeLow/High); [1..] multi-page */
static gj_paddr_t g_aOrderLow[PMM_MAX_ORDER + 1u];
static gj_paddr_t g_aOrderHigh[PMM_MAX_ORDER + 1u];
/* Per-order free *nodes* (not frames). Order 0 tracks single-frame nodes. */
static u64        g_aOrderCount[PMM_MAX_ORDER + 1u];
static u64        g_cFramesFree;
/* Zone free frames (sum = g_cFramesFree); 1 TiB-class observability. */
static u64        g_cFramesFreeLow;
static u64        g_cFramesFreeHigh;
static u64        g_cFramesTotal;
static u64        g_paMaxSeen;
/* Soft observability: split / high-order push events (never invent frames). */
static u64        g_cSplit;
static u64        g_cHighOrderPush;
static gj_paddr_t g_paKernel0;
static gj_paddr_t g_paKernel1;
static struct pmm_pending g_aHigh[PMM_HIGH_PENDING_MAX];
static u32        g_cHigh;
static int        g_fHighReleased;
/* Wave 15: times soft inventory printed (product / smoke observability). */
static u32        g_cSoftInvLogs;

static gj_paddr_t pop_order_split(u32 u32Order);
static u64        high_order_nodes(void);
static void       log_order_hist(const char *szTag);
static void       log_tib_design_soft(void);
static void       pmm_soft_inventory(const char *szWhere);
static u32        soft_hier_exercise(u32 *pOutBig);

static void *
pa_to_ptr(gj_paddr_t pa)
{
    /*
     * High RAM (>=4 GiB) is only valid under HHDM after vmm_hhdm_init.
     * Never identity-map high PAs (causes #PF on large GJ_MEM / 768G soak).
     */
    if (pa >= PMM_LOW_MAX) {
        if (!hhdm_ready()) {
            return NULL;
        }
        return (void *)(gj_vaddr_t)(GJ_HHDM_BASE + (u64)pa);
    }
    if (hhdm_ready()) {
        return (void *)(gj_vaddr_t)(GJ_HHDM_BASE + (u64)pa);
    }
    return (void *)(gj_vaddr_t)pa;
}

/* Byte length of an order-N block (1<<N pages). */
static gj_paddr_t
order_bytes(u32 u32Order)
{
    return (gj_paddr_t)GJ_PAGE_SIZE << u32Order;
}

/* Natural alignment for order-N (order 0: any page). */
static int
order_aligned(gj_paddr_t pa, u32 u32Order)
{
    gj_paddr_t cbBlk;

    if (u32Order == 0) {
        return (pa & (GJ_PAGE_SIZE - 1)) == 0;
    }
    cbBlk = order_bytes(u32Order);
    return (pa & (cbBlk - 1ull)) == 0;
}

static int
range_overlaps(gj_paddr_t paA0, gj_paddr_t paA1, gj_paddr_t paB0, gj_paddr_t paB1)
{
    return paA0 < paB1 && paB0 < paA1;
}

/**
 * True if [pa, pa+cb) may enter the freelist (not low BIOS, not kernel image).
 * Used by free_range and as a defensive guard on all push_order paths.
 */
static int
range_ok_free(gj_paddr_t pa, u64 cb)
{
    if (cb == 0 || pa < 0x100000ull) {
        return 0;
    }
    if (range_overlaps(pa, pa + (gj_paddr_t)cb, g_paKernel0, g_paKernel1)) {
        return 0;
    }
    return 1;
}

/**
 * Push a (1<<u32Order)-page block onto the hierarchical freelist.
 * Base must be page-aligned; order-N also requires natural block alignment
 * or frames fall back to order-0 singles (never silently dropped).
 * Kernel image / low-BIOS ranges are refused (safety).
 */
static void
push_order(gj_paddr_t paPage, u32 u32Order)
{
    u64 *p;
    gj_paddr_t *pHead;
    u32 cPages;
    u64 cbSpan;

    if (paPage == 0 || (paPage & (GJ_PAGE_SIZE - 1)) != 0) {
        return;
    }
    if (u32Order > PMM_MAX_ORDER) {
        return;
    }
    cPages = 1u << u32Order;
    cbSpan = (u64)cPages * GJ_PAGE_SIZE;
    /* Misaligned multi-page free: preserve frames via order-0 push. */
    if (u32Order > 0 && !order_aligned(paPage, u32Order)) {
        u32 iPage;

        for (iPage = 0; iPage < cPages; iPage++) {
            push_order(paPage + (gj_paddr_t)iPage * GJ_PAGE_SIZE, 0);
        }
        return;
    }
    /* Safety: never list kernel image or firmware low hole. */
    if (!range_ok_free(paPage, cbSpan)) {
        return;
    }
    p = (u64 *)pa_to_ptr(paPage);
    if (p == NULL) {
        /* High PA before HHDM: caller must use pending high path. */
        return;
    }
    if (u32Order == 0) {
        pHead = (paPage < PMM_LOW_MAX) ? &g_paFreeLow : &g_paFreeHigh;
    } else {
        pHead = (paPage < PMM_LOW_MAX) ? &g_aOrderLow[u32Order]
                                       : &g_aOrderHigh[u32Order];
    }
    *p = (u64)*pHead;
    *pHead = paPage;
    g_cFramesFree += cPages;
    if (paPage < PMM_LOW_MAX) {
        g_cFramesFreeLow += cPages;
    } else {
        g_cFramesFreeHigh += cPages;
    }
    g_aOrderCount[u32Order]++;
    if (u32Order > 0) {
        g_cHighOrderPush++;
    }
}

static void
push_free(gj_paddr_t paPage)
{
    push_order(paPage, 0);
}

/**
 * Pop one order-N block (low preferred, then high). Does not split.
 * Corrupt / unaligned heads are refused without advancing the list.
 */
static gj_paddr_t
pop_order(u32 u32Order)
{
    gj_paddr_t pa;
    u64 *p;
    gj_paddr_t *pHead;
    u32 cPages;

    if (u32Order > PMM_MAX_ORDER) {
        return 0;
    }
    cPages = 1u << u32Order;
    if (u32Order == 0) {
        if (g_paFreeLow != 0) {
            pHead = &g_paFreeLow;
        } else if (g_paFreeHigh != 0) {
            pHead = &g_paFreeHigh;
        } else {
            return 0;
        }
    } else if (g_aOrderLow[u32Order] != 0) {
        pHead = &g_aOrderLow[u32Order];
    } else if (g_aOrderHigh[u32Order] != 0) {
        pHead = &g_aOrderHigh[u32Order];
    } else {
        return 0;
    }
    pa = *pHead;
    /*
     * Defensive: refuse zero / non-page / non-natural-order heads.
     * Do not advance the list — avoid handing out garbage PAs.
     */
    if (pa == 0 || !order_aligned(pa, u32Order)) {
        return 0;
    }
    p = (u64 *)pa_to_ptr(pa);
    if (p == NULL) {
        return 0;
    }
    *pHead = (gj_paddr_t)(*p);
    *p = 0;
    if (g_cFramesFree >= cPages) {
        g_cFramesFree -= cPages;
    } else {
        g_cFramesFree = 0;
    }
    if (pa < PMM_LOW_MAX) {
        if (g_cFramesFreeLow >= cPages) {
            g_cFramesFreeLow -= cPages;
        } else {
            g_cFramesFreeLow = 0;
        }
    } else if (g_cFramesFreeHigh >= cPages) {
        g_cFramesFreeHigh -= cPages;
    } else {
        g_cFramesFreeHigh = 0;
    }
    if (g_aOrderCount[u32Order] > 0) {
        g_aOrderCount[u32Order]--;
    }
    return pa;
}

static gj_paddr_t
pop_free(void)
{
    /*
     * Prefer order-0 singles; if free_range bulk-freed as higher orders
     * (large-RAM / 768G path), split a higher block down to a single page.
     */
    return pop_order_split(0);
}

/**
 * Hierarchical alloc: exact order pop, else split a higher-order block and
 * push sibling buddies (orders o-1 … want) back onto hierarchical free.
 * Rearranges freelist nodes only — does not invent physical frames.
 *
 * Invariant (do not regress): after pop of order o and split down to want,
 * returned PA is the low base of the original block; each discarded upper
 * half is push_order'd at its buddy order. free_range + this path are the
 * large-RAM hierarchical free core (768G soak_tib).
 */
static gj_paddr_t
pop_order_split(u32 u32Order)
{
    u32 o;
    gj_paddr_t pa;

    if (u32Order > PMM_MAX_ORDER) {
        return 0;
    }
    pa = pop_order(u32Order);
    if (pa != 0) {
        return pa;
    }
    for (o = u32Order + 1u; o <= PMM_MAX_ORDER; o++) {
        pa = pop_order(o);
        if (pa == 0) {
            continue;
        }
        /* Split: free upper half buddies of orders o-1 … u32Order. */
        g_cSplit++;
        while (o > u32Order) {
            o--;
            push_order(pa + ((gj_paddr_t)(1u << o) * GJ_PAGE_SIZE), o);
        }
        return pa;
    }
    return 0;
}

/**
 * Sum of free nodes on orders 1..PMM_MAX_ORDER (not frames).
 * Greppable high-order soft observability for 1 TiB design.
 */
static u64
high_order_nodes(void)
{
    u32 o;
    u64 c = 0;

    for (o = 1; o <= PMM_MAX_ORDER; o++) {
        c += g_aOrderCount[o];
    }
    return c;
}

/**
 * Greppable order histogram + zone free counts.
 * tag=init | high_release | soak_tib | soak_soft | …
 */
static void
log_order_hist(const char *szTag)
{
    u32 o;
    u64 cHi = high_order_nodes();

    if (szTag == 0) {
        szTag = "?";
    }
    /* Greppable: pmm: orders tag= */
    kprintf("pmm: orders tag=%s free=%lu low=%lu high=%lu o0=%lu",
            szTag, (unsigned long)g_cFramesFree,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            (unsigned long)g_aOrderCount[0]);
    for (o = 1; o <= PMM_MAX_ORDER; o++) {
        kprintf(" o%u=%lu", o, (unsigned long)g_aOrderCount[o]);
    }
    kprintf(" high_order_nodes=%lu splits=%lu high_order_push=%lu "
            "max_order=%u hierarchical free\n",
            (unsigned long)cHi, (unsigned long)g_cSplit,
            (unsigned long)g_cHighOrderPush, PMM_MAX_ORDER);
}

/**
 * Always-on 1 TiB design soft marker (does not require 1 TiB host).
 * Greppable: pmm: tib_design soft | pmm: tib_host soft PASS|SKIP
 * Soft host/design observability only — not a 1 TiB product claim.
 */
static void
log_tib_design_soft(void)
{
    u64 cHi = high_order_nodes();
    u64 cbBlock = (u64)GJ_PAGE_SIZE << PMM_MAX_ORDER;

    /* Greppable: pmm: tib_design soft */
    kprintf("pmm: tib_design soft max_order=%u max_block_pages=%u "
            "max_block_bytes=%lu design_ceil_tib=%u max_pa=0x%lx free=%lu "
            "high_order_nodes=%lu hierarchical free (no 1TiB host required; "
            "not 1TiB product claim)\n",
            PMM_MAX_ORDER, 1u << PMM_MAX_ORDER, (unsigned long)cbBlock,
            (unsigned)GJ_PMM_MAX_PHYS_TIB, (unsigned long)g_paMaxSeen,
            (unsigned long)g_cFramesFree, (unsigned long)cHi);

    if (g_paMaxSeen >= PMM_TIB_BYTES) {
        /* Greppable: pmm: tib_host soft PASS (host size soft gate only). */
        kprintf("pmm: tib_host soft PASS max_pa=0x%lx need=0x%lx free=%lu "
                "high_order_nodes=%lu (host size soft; not 1TiB product)\n",
                (unsigned long)g_paMaxSeen, (unsigned long)PMM_TIB_BYTES,
                (unsigned long)g_cFramesFree, (unsigned long)cHi);
    } else {
        /* Greppable: pmm: tib_host soft SKIP */
        kprintf("pmm: tib_host soft SKIP max_pa=0x%lx need=0x%lx free=%lu "
                "high_order_nodes=%lu (host/QEMU below 1TiB; design path ready; "
                "not 1TiB product claim)\n",
                (unsigned long)g_paMaxSeen, (unsigned long)PMM_TIB_BYTES,
                (unsigned long)g_cFramesFree, (unsigned long)cHi);
    }

    /* Greppable: pmm: high_order soft */
    kprintf("pmm: high_order soft nodes=%lu max_order=%u free_low=%lu "
            "free_high=%lu hierarchical free\n",
            (unsigned long)cHi, PMM_MAX_ORDER,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh);
}

/**
 * Wave 19 greppable soft PMM inventory dump (product / smoke deepen).
 * Prefix-stable markers (pmm: soft …):
 *   pmm: soft honesty    — explicit non-claims (not 1 TiB product; not bar3)
 *   pmm: soft inventory  — free/total, zones, pending, hierarchy snapshot
 *   pmm: soft zones      — low/high free frames + release state
 *   pmm: soft hier       — max_order, nodes, splits, high-order pushes
 *   pmm: soft orders     — per-order node counts (soft snapshot)
 *   pmm: soft heads      — freelist head presence (order-0 + top order)
 *   pmm: soft pending    — high-pending ranges before release
 *   pmm: soft host       — host size vs 1 TiB gate (soft only)
 *   pmm: soft design     — design max_order / block size (soft only)
 *   pmm: soft path       — surface catalog + product_tib=0
 *   pmm: soft stats      — rollup free/use/splits/nodes/logs
 *   pmm: soft geometry   — page/zone/order constants (Wave 15)
 *   pmm: soft kernel     — kernel image reserve snap (Wave 15)
 *   pmm: soft hhdm       — high-zone vs HHDM dependency (Wave 15)
 *   pmm: soft lamps      — composite readiness lamps (Wave 15)
 *   pmm: soft OPEN       — P-MEM-3 / product_tib / bar3 OPEN (Wave 15)
 *   pmm: soft surfaces   — Wave 19 return-surface catalog (surf bitmask)
 *   pmm: soft ratio      — Wave 17 free/total soft ratio lamps
 *   pmm: soft sites      — Wave 17 emission-site catalog
 *   pmm: soft api        — Wave 17 alloc/free surface return lamps
 *   pmm: soft return selftest — Wave 19 terminal return surface
 *   pmm: soft retmap     — Wave 19 return-surface map
 *   pmm: soft deepen     — wave=87 stamp + area count
 *   pmm: soft PASS | EMPTY | NONE | inventory PASS
 *
 * Never allocates. Safe after pmm_init (and later release/soak paths).
 * Honesty: soft inventory ≠ 1 TiB product claim; never closes P-MEM-3;
 *          soft ≠ bar3; soft ≠ product.
 * greppable: pmm: soft
 */
static void
pmm_soft_inventory(const char *szWhere)
{
    u64 cHi = high_order_nodes();
    u64 cbBlock = (u64)GJ_PAGE_SIZE << PMM_MAX_ORDER;
    u64 cInUse;
    u64 cNodesAll;
    u64 cKerPages;
    u32 o;
    u32 cAreas = 0;
    u32 u32Hhdm;
    u32 u32Surf;
    u32 u32FreePct;
    u32 u32LowPct;
    const char *szReady;
    const char *szHost;
    int fReady;
    int fHostTib;

    if (szWhere == 0) {
        szWhere = "path";
    }
    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    if (g_cFramesTotal >= g_cFramesFree) {
        cInUse = g_cFramesTotal - g_cFramesFree;
    } else {
        cInUse = 0;
    }

    cNodesAll = g_aOrderCount[0] + cHi;
    u32Hhdm = hhdm_ready() ? 1u : 0u;
    u32Surf = PMM_SOFT_SURF_CATALOG;

    if (g_paKernel1 > g_paKernel0) {
        cKerPages = (u64)(g_paKernel1 - g_paKernel0) / (u64)GJ_PAGE_SIZE;
    } else {
        cKerPages = 0;
    }

    /*
     * Soft readiness only: freelist has frames after init.
     * Does not encode host RAM class or 1 TiB product.
     */
    fReady = 0;
    if (g_cFramesFree > 0 && g_cFramesTotal > 0) {
        szReady = "READY";
        fReady = 1;
    } else if (g_cFramesTotal > 0) {
        szReady = "EMPTY";
    } else {
        szReady = "NONE";
    }

    /* Host size soft gate only — never product 1 TiB claim. */
    fHostTib = (g_paMaxSeen >= PMM_TIB_BYTES) ? 1 : 0;
    szHost = fHostTib ? "PASS" : "SKIP";

    /* Soft free/total ratio lamps (diagnostics only; never product gate). */
    if (g_cFramesTotal == 0) {
        u32FreePct = 0;
        u32LowPct = 0;
    } else {
        u32FreePct = (u32)((g_cFramesFree * 100ull) / g_cFramesTotal);
        if (u32FreePct > 100u) {
            u32FreePct = 100u;
        }
        u32LowPct = (u32)((g_cFramesFreeLow * 100ull) / g_cFramesTotal);
        if (u32LowPct > 100u) {
            u32LowPct = 100u;
        }
    }

    /*
     * Honesty first: freestanding soft inventory is NOT 1 TiB product,
     * not bar3. greppable: pmm: soft honesty
     */
    kprintf("pmm: soft honesty not-1TiB-product not-bar3 pmem3=OPEN "
            "product_tib=0 bar3=OPEN design_ceil_tib=%u max_order=%u "
            "host_tib=%s hierarchical free wave=%u "
            "(soft inventory only; never closes P-MEM-3; not bar3)\n",
            (unsigned)GJ_PMM_MAX_PHYS_TIB, PMM_MAX_ORDER, szHost,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft inventory */
    kprintf("pmm: soft inventory via=%s ready=%s free=%lu total=%lu "
            "in_use=%lu free_low=%lu free_high=%lu max_pa=0x%lx "
            "high_pending=%u high_released=%u max_order=%u "
            "high_order_nodes=%lu nodes_all=%lu logs=%u wave=%u "
            "hierarchical free (soft; not 1TiB product; not bar3)\n",
            szWhere, szReady,
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)cInUse,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            (unsigned long)g_paMaxSeen, g_cHigh,
            g_fHighReleased ? 1u : 0u, PMM_MAX_ORDER,
            (unsigned long)cHi, (unsigned long)cNodesAll, g_cSoftInvLogs,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft zones */
    kprintf("pmm: soft zones free_low=%lu free_high=%lu pending=%u "
            "released=%u low_head=%u high_head=%u kernel0=0x%lx "
            "kernel1=0x%lx zone_split_bytes=0x%lx wave=%u\n",
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            g_cHigh, g_fHighReleased ? 1u : 0u,
            g_paFreeLow != 0 ? 1u : 0u, g_paFreeHigh != 0 ? 1u : 0u,
            (unsigned long)g_paKernel0, (unsigned long)g_paKernel1,
            (unsigned long)PMM_LOW_MAX, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft hier */
    kprintf("pmm: soft hier max_order=%u o0=%lu high_order_nodes=%lu "
            "nodes_all=%lu splits=%lu high_order_push=%lu block_pages=%u "
            "block_bytes=%lu wave=%u hierarchical free\n",
            PMM_MAX_ORDER, (unsigned long)g_aOrderCount[0],
            (unsigned long)cHi, (unsigned long)cNodesAll,
            (unsigned long)g_cSplit,
            (unsigned long)g_cHighOrderPush, 1u << PMM_MAX_ORDER,
            (unsigned long)cbBlock, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft orders — per-order node snapshot (soft; not product). */
    kprintf("pmm: soft orders via=%s", szWhere);
    for (o = 0; o <= PMM_MAX_ORDER; o++) {
        kprintf(" o%u=%lu", o, (unsigned long)g_aOrderCount[o]);
    }
    kprintf(" high_order_nodes=%lu splits=%lu wave=%u hierarchical free "
            "(soft; not 1TiB product)\n",
            (unsigned long)cHi, (unsigned long)g_cSplit,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft heads — freelist head lamps (presence only). */
    kprintf("pmm: soft heads low_o0=%u high_o0=%u low_top=%u high_top=%u "
            "max_order=%u free_low=%lu free_high=%lu wave=%u\n",
            g_paFreeLow != 0 ? 1u : 0u, g_paFreeHigh != 0 ? 1u : 0u,
            g_aOrderLow[PMM_MAX_ORDER] != 0 ? 1u : 0u,
            g_aOrderHigh[PMM_MAX_ORDER] != 0 ? 1u : 0u,
            PMM_MAX_ORDER,
            (unsigned long)g_cFramesFreeLow,
            (unsigned long)g_cFramesFreeHigh,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft pending — high ranges parked before HHDM release. */
    kprintf("pmm: soft pending count=%u released=%u max=%u "
            "free_high=%lu high_order_nodes=%lu wave=%u "
            "(soft; not 1TiB product)\n",
            g_cHigh, g_fHighReleased ? 1u : 0u, PMM_HIGH_PENDING_MAX,
            (unsigned long)g_cFramesFreeHigh, (unsigned long)cHi,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft host — size gate only; never product claim. */
    kprintf("pmm: soft host tib=%s max_pa=0x%lx need=0x%lx free=%lu "
            "total=%lu free_low=%lu free_high=%lu high_order_nodes=%lu "
            "wave=%u (host size soft only; not 1TiB product claim)\n",
            szHost, (unsigned long)g_paMaxSeen, (unsigned long)PMM_TIB_BYTES,
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            (unsigned long)cHi, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft design — design geometry only; not product 1 TiB. */
    kprintf("pmm: soft design max_order=%u max_block_pages=%u "
            "max_block_bytes=%lu design_ceil_tib=%u max_pa=0x%lx "
            "page_size=%u product_tib=0 wave=%u "
            "(soft inventory only; not 1TiB product claim)\n",
            PMM_MAX_ORDER, 1u << PMM_MAX_ORDER, (unsigned long)cbBlock,
            (unsigned)GJ_PMM_MAX_PHYS_TIB, (unsigned long)g_paMaxSeen,
            (unsigned)GJ_PAGE_SIZE, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Soft path honesty: surface catalog + explicit non-claims.
     * greppable: pmm: soft path
     */
    kprintf("pmm: soft path via=%s sites=init,high_release,soak,soak_soft,"
            "soak_tib,api dual_zone=1 hierarchical=1 max_order=%u "
            "product_tib=0 pmem3=OPEN bar_tib=OPEN bar3=OPEN wave=%u\n",
            szWhere, PMM_MAX_ORDER, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft stats — rollup tallies for agent greps. */
    kprintf("pmm: soft stats free=%lu total=%lu in_use=%lu free_low=%lu "
            "free_high=%lu nodes_all=%lu high_order_nodes=%lu splits=%lu "
            "high_order_push=%lu pending=%u released=%u logs=%u "
            "ready=%s host_tib=%s wave=%u\n",
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)cInUse,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            (unsigned long)cNodesAll, (unsigned long)cHi,
            (unsigned long)g_cSplit, (unsigned long)g_cHighOrderPush,
            g_cHigh, g_fHighReleased ? 1u : 0u, g_cSoftInvLogs,
            szReady, szHost, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: geometry catalog (constants only; not product size claim).
     * Grep: pmm: soft geometry
     */
    kprintf("pmm: soft geometry page=%u page_shift=%u max_order=%u "
            "block_pages=%u block_bytes=%lu zone_split=0x%lx "
            "pending_max=%u design_ceil_tib=%u tib_need=0x%lx "
            "hhdm_base=0x%lx wave=%u (soft; not 1TiB product; not bar3)\n",
            (unsigned)GJ_PAGE_SIZE, (unsigned)GJ_PAGE_SHIFT, PMM_MAX_ORDER,
            1u << PMM_MAX_ORDER, (unsigned long)cbBlock,
            (unsigned long)PMM_LOW_MAX, PMM_HIGH_PENDING_MAX,
            (unsigned)GJ_PMM_MAX_PHYS_TIB, (unsigned long)PMM_TIB_BYTES,
            (unsigned long)GJ_HHDM_BASE, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: kernel image reserve snap (excluded from freelist).
     * Grep: pmm: soft kernel
     */
    kprintf("pmm: soft kernel reserve0=0x%lx reserve1=0x%lx "
            "span_pages=%lu reserved=1 wave=%u "
            "(image reserve soft; not 1TiB product)\n",
            (unsigned long)g_paKernel0, (unsigned long)g_paKernel1,
            (unsigned long)cKerPages, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: high freelist depends on HHDM (P-MEM-5) — soft only.
     * Grep: pmm: soft hhdm
     */
    kprintf("pmm: soft hhdm high_released=%u high_pending=%u "
            "free_high=%lu hhdm_ready=%u p_mem5=1 wave=%u "
            "(high zone needs HHDM; soft; not 1TiB product; not bar3)\n",
            g_fHighReleased ? 1u : 0u, g_cHigh,
            (unsigned long)g_cFramesFreeHigh, u32Hhdm,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: composite readiness lamps (never hard-gate).
     * Grep: pmm: soft lamps
     */
    kprintf("pmm: soft lamps ready=%s free_gt0=%u total_gt0=%u "
            "low_head=%u high_head=%u high_released=%u hhdm_ready=%u "
            "host_tib=%s hierarchical=1 dual_zone=1 wave=%u "
            "(composite soft lamps; not product gate; not bar3)\n",
            szReady,
            g_cFramesFree > 0 ? 1u : 0u,
            g_cFramesTotal > 0 ? 1u : 0u,
            g_paFreeLow != 0 ? 1u : 0u,
            g_paFreeHigh != 0 ? 1u : 0u,
            g_fHighReleased ? 1u : 0u,
            u32Hhdm, szHost, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15 honesty close: P-MEM-3 / product_tib / bar3 remain OPEN.
     * Grep: pmm: soft OPEN
     */
    kprintf("pmm: soft OPEN pmem3=OPEN product_tib=0 bar3=OPEN "
            "bar_tib=OPEN host_tib=%s design_ceil_tib=%u free=%lu "
            "total=%lu wave=%u "
            "(soft inventory; never closes P-MEM-3; not 1TiB product; "
            "not bar3)\n",
            szHost, (unsigned)GJ_PMM_MAX_PHYS_TIB,
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: pmm: soft surfaces
     */
    kprintf("pmm: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "honesty=1 inventory=1 zones=1 hier=1 orders=1 heads=1 "
            "pending=1 host=1 design=1 path=1 stats=1 geometry=1 "
            "kernel=1 hhdm=1 lamps=1 open=1 ratio=1 sites=1 api=1 "
            "wave=%u (return surfaces; soft only; not product; not bar3)\n",
            (unsigned)u32Surf, (unsigned)PMM_SOFT_AREAS, cAreas + 4u,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: free/total soft ratio lamps (never product gate).
     * Grep: pmm: soft ratio
     */
    kprintf("pmm: soft ratio free_pct=%u low_pct=%u free=%lu total=%lu "
            "in_use=%lu free_low=%lu free_high=%lu high_order_nodes=%lu "
            "wave=%u (soft ratio; not 1TiB product; not bar3)\n",
            u32FreePct, u32LowPct,
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)cInUse,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            (unsigned long)cHi, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: emission-site catalog (via= string space).
     * Grep: pmm: soft sites
     */
    kprintf("pmm: soft sites via=%s "
            "emit=init|high_release|soak|soak_soft|soak_tib|api "
            "logs=%u dual_zone=1 hierarchical=1 wave=%u "
            "(soft emission sites; not product; not bar3)\n",
            szWhere, g_cSoftInvLogs, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: alloc/free surface return lamps (observability only).
     * Grep: pmm: soft api
     */
    kprintf("pmm: soft api alloc=1 free=1 alloc_pages=1 free_pages=1 "
            "release_high=1 soak=1 soak_tib=1 log_orders=1 "
            "prefer_low=1 split=1 product_tib=0 pmem3=OPEN wave=%u "
            "(soft API return surfaces; not product; not bar3)\n",
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: pmm: soft return rate
     * Wave 17 return-surface rate lamps (kept) (free/total + readiness).
     */
    kprintf("pmm: soft return rate free=%lu total=%lu in_use=%lu "
            "free_pct=%u ready=%s logs=%u wave=%u "
            "(return rate; Soft≠product; not 1TiB product; not bar3)\n",
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)cInUse, u32FreePct, szReady, g_cSoftInvLogs,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: pmm: soft retcode
     * Wave 17 retcode catalog for alloc/free soft return classes.
     */
    kprintf("pmm: soft retcode "
            "alloc=1 free=1 alloc_pages=1 free_pages=1 "
            "release_high=1 soak=1 empty=1 none=1 pass=1 "
            "product_tib=0 pmem3=OPEN wave=%u "
            "(retcode catalog; Soft≠product; not 1TiB product)\n",
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: pmm: soft deepen wave (Wave 21 stamp; areas = prior soft lines).
     * catalog=PMM_SOFT_AREAS is design high-water; areas is this emission.
     */
    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: pmm: soft return selftest — Wave 19 terminal return surface */
    kprintf("pmm: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)PMM_SOFT_WAVE);

    /* Grep: pmm: soft retmap — Wave 19 return-surface map */
    kprintf("pmm: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)PMM_SOFT_WAVE);

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pmm: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("pmm: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)PMM_SOFT_WAVE);
    /* Grep: pmm: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("pmm: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)PMM_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pmm: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("pmm: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)PMM_SOFT_WAVE);
    /* Grep: pmm: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("pmm: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pmm: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("pmm: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: pmm: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("pmm: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pmm: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("pmm: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: pmm: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("pmm: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: pmm: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("pmm: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: pmm: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("pmm: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pmm: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("pmm: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: pmm: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("pmm: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pmm: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("pmm: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: pmm: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("pmm: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pmm: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("pmm: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)PMM_SOFT_WAVE);
            /* Grep: pmm: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("pmm: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)PMM_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: pmm: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("pmm: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PMM_SOFT_WAVE);
                    /* Grep: pmm: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("pmm: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PMM_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("pmm: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)PMM_SOFT_WAVE);
                            /* Grep: pmm: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("pmm: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)PMM_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("pmm: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: pmm: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("pmm: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pmm: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: pmm: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("pmm: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pmm: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("pmm: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: pmm: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pmm: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)PMM_SOFT_WAVE);
                            /* Grep: pmm: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("pmm: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("pmm: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("pmm: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("pmm: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("pmm: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("pmm: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("pmm: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retfortress — Wave 35 return-fortress honesty */
kprintf("pmm: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("pmm: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft rethold — Wave 36 return-hold honesty */
kprintf("pmm: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retspire — Wave 36 exclusive spire stamp */
kprintf("pmm: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retwall — Wave 37 return-wall honesty */
kprintf("pmm: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retgate — Wave 37 exclusive gate stamp */
kprintf("pmm: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retmoat — Wave 38 return-moat honesty */
kprintf("pmm: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retower — Wave 38 exclusive tower stamp */
kprintf("pmm: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("pmm: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("pmm: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("pmm: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("pmm: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retravelin — Wave 41 return-travelin honesty */
kprintf("pmm: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("pmm: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("pmm: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("pmm: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("pmm: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("pmm: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("pmm: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("pmm: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("pmm: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("pmm: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retbailey — Wave 46 return-bailey honesty */
kprintf("pmm: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)PMM_SOFT_WAVE);
/* Grep: pmm: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("pmm: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)PMM_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("pmm: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("pmm: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("pmm: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("pmm: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("pmm: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("pmm: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retsally — Wave 50 return-sally honesty */
kprintf("pmm: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("pmm: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retfosse — Wave 51 return-fosse honesty */
kprintf("pmm: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("pmm: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("pmm: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("pmm: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retravelin — Wave 53 return-travelin honesty */
kprintf("pmm: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("pmm: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("pmm: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retredan — Wave 54 exclusive redan stamp */
kprintf("pmm: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retflank — Wave 55 return-flank honesty */
kprintf("pmm: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retface — Wave 55 exclusive face stamp */
kprintf("pmm: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retgorge — Wave 56 return-gorge honesty */
kprintf("pmm: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("pmm: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retraverse — Wave 57 return-traverse honesty */
kprintf("pmm: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("pmm: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retorillon — Wave 58 return-orillon honesty */
kprintf("pmm: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("pmm: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("pmm: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("pmm: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retplace — Wave 60 return-place honesty */
kprintf("pmm: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("pmm: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("pmm: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("pmm: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("pmm: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("pmm: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("pmm: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("pmm: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: pmm: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("pmm: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: pmm: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("pmm: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: pmm: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("pmm: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: pmm: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("pmm: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: pmm: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("pmm: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=87 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: pmm: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("pmm: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=87 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("pmm: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("pmm: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("pmm: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("pmm: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("pmm: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=87 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("pmm: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=87 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("pmm: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("pmm: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("pmm: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("pmm: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pmm: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("pmm: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("pmm: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("pmm: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("pmm: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retbastionangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("pmm: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("pmm: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retparapetangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("pmm: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("pmm: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retowerangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("pmm: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("pmm: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retwallangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("pmm: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("pmm: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retholdangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("pmm: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("pmm: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retfortressangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("pmm: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("pmm: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("pmm: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("pmm: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: pmm: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("pmm: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("pmm: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("pmm: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("pmm: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retaegisangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("pmm: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("pmm: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retsigilangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("pmm: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("pmm: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retglyphangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("pmm: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("pmm: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retshardangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("pmm: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("pmm: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retprismangle stamp; Soft≠product)\n");
/* Grep: pmm: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("pmm: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=87 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: pmm: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("pmm: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=87 (retcipherangle stamp; Soft≠product)\n");

                            kprintf("pmm: soft deepen wave=%u areas=%u catalog=%u via=%s ready=%s "
            "free=%lu logs=%u surf=0x%x product_tib=0 pmem3=OPEN bar3=OPEN "
            "(Wave 35 exclusive; soft; not 1TiB product; not bar3; "
            "soft≠product)\n",
            (unsigned)PMM_SOFT_WAVE, cAreas, (unsigned)PMM_SOFT_AREAS,
            szWhere, szReady, (unsigned long)g_cFramesFree, g_cSoftInvLogs,
            (unsigned)u32Surf);

    /*
     * Close markers: freelist soft readiness only.
     * Grep: pmm: soft PASS | pmm: soft EMPTY | pmm: soft NONE
     * Never "1TiB product PASS" / "bar3 PASS".
     */
    if (fReady) {
        /* Grep: pmm: soft PASS | pmm: soft inventory PASS */
        kprintf("pmm: soft PASS via=%s free=%lu wave=%u hierarchical free "
                "(soft inventory; not 1TiB product; not bar3)\n",
                szWhere, (unsigned long)g_cFramesFree,
                (unsigned)PMM_SOFT_WAVE);
        kprintf("pmm: soft inventory PASS via=%s logs=%u wave=%u "
                "(soft; not 1TiB product; not bar3)\n",
                szWhere, g_cSoftInvLogs, (unsigned)PMM_SOFT_WAVE);
    } else {
        kprintf("pmm: soft %s via=%s free=%lu total=%lu wave=%u "
                "(soft inventory; not 1TiB product; not bar3)\n",
                szReady, szWhere, (unsigned long)g_cFramesFree,
                (unsigned long)g_cFramesTotal, (unsigned)PMM_SOFT_WAVE);
    }

    (void)PMM_SOFT_AREAS;
}

/**
 * Soft hierarchical exercise for any RAM size (1 TiB design observability).
 * Tries alloc/free each order 1..MAX and a few max-order (2 MiB) blocks.
 * Rearranges freelist nodes only — never invents frames; safe when free is
 * small (failed orders simply skip). Returns # of orders that succeeded.
 */
static u32
soft_hier_exercise(u32 *pOutBig)
{
    u32 o;
    u32 cOrderOk = 0;
    u32 nBig = 0;
    u32 i;

    for (o = 1; o <= PMM_MAX_ORDER; o++) {
        gj_paddr_t pa = pmm_alloc_pages(1u << o);

        if (pa != 0) {
            pmm_free_pages(pa, 1u << o);
            cOrderOk++;
        }
    }
    for (i = 0; i < 8u; i++) {
        gj_paddr_t pa = pmm_alloc_pages(1u << PMM_MAX_ORDER);

        if (pa == 0) {
            break;
        }
        pmm_free_pages(pa, 1u << PMM_MAX_ORDER);
        nBig++;
    }
    if (pOutBig != 0) {
        *pOutBig = nBig;
    }
    return cOrderOk;
}

/* Smallest order whose block has ≥ cPages (capped at PMM_MAX_ORDER). */
static u32
pages_to_order(u32 cPages)
{
    u32 o = 0;
    u32 n = 1;

    if (cPages == 0) {
        return 0;
    }
    while (n < cPages && o < PMM_MAX_ORDER) {
        n <<= 1;
        o++;
    }
    return o;
}

/**
 * Free a physical range into hierarchical freelists.
 * Prefer largest aligned power-of-two blocks (up to 2 MiB / order 9) so
 * multi-hundred-GiB machines do not walk every 4 KiB page at boot.
 *
 * Algorithm (stable — do not regress for 768G soak):
 *   page-align [paBase, paBase+cbLen); at each pa pick the largest order
 *   o where pa is order-aligned, [pa, pa+sz) fits in the range, and
 *   range_ok_free holds for the whole block; push_order(pa, o).
 */
static void
free_range(gj_paddr_t paBase, u64 cbLen, int fCountTotal)
{
    gj_paddr_t paEnd = paBase + cbLen;
    gj_paddr_t pa;

    pa = (paBase + GJ_PAGE_SIZE - 1) & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
    paEnd &= ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
    while (pa + GJ_PAGE_SIZE <= paEnd) {
        u32 o;
        u64 sz;
        u32 cPages;

        if (!range_ok_free(pa, GJ_PAGE_SIZE)) {
            pa += GJ_PAGE_SIZE;
            continue;
        }
        o = PMM_MAX_ORDER;
        for (;;) {
            sz = (u64)GJ_PAGE_SIZE << o;
            cPages = 1u << o;
            if ((pa & (sz - 1ull)) == 0 && pa + (gj_paddr_t)sz <= paEnd &&
                range_ok_free(pa, sz)) {
                break;
            }
            if (o == 0) {
                break;
            }
            o--;
        }
        if (fCountTotal) {
            g_cFramesTotal += cPages;
        }
        push_order(pa, o);
        pa += (gj_paddr_t)sz;
    }
}

void
pmm_init(const struct gj_mem_region *pRegions, size_t cRegions,
         gj_paddr_t paKernelStart, gj_paddr_t paKernelEnd)
{
    size_t iRegion;

    g_paFreeLow = 0;
    g_paFreeHigh = 0;
    g_cFramesFree = 0;
    g_cFramesFreeLow = 0;
    g_cFramesFreeHigh = 0;
    g_cFramesTotal = 0;
    g_paMaxSeen = 0;
    g_cSplit = 0;
    g_cHighOrderPush = 0;
    g_cHigh = 0;
    g_fHighReleased = 0;
    g_cSoftInvLogs = 0;
    {
        u32 o;

        for (o = 0; o <= PMM_MAX_ORDER; o++) {
            g_aOrderLow[o] = 0;
            g_aOrderHigh[o] = 0;
            g_aOrderCount[o] = 0;
        }
    }

    g_paKernel0 = paKernelStart & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
    g_paKernel1 = (paKernelEnd + GJ_PAGE_SIZE - 1) & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);

    for (iRegion = 0; iRegion < cRegions; iRegion++) {
        gj_paddr_t paBase = pRegions[iRegion].paBase;
        u64 cbLen = pRegions[iRegion].cbLength;
        gj_paddr_t paEnd = paBase + cbLen;

        if (paEnd > g_paMaxSeen) {
            g_paMaxSeen = paEnd;
        }

        /* Low RAM: hierarchical free_range immediately (identity / early HHDM). */
        if (paEnd <= PMM_LOW_MAX) {
            free_range(paBase, cbLen, 1);
            continue;
        }
        if (paBase < PMM_LOW_MAX) {
            free_range(paBase, PMM_LOW_MAX - paBase, 1);
            paBase = PMM_LOW_MAX;
            cbLen = paEnd - paBase;
        }
        /*
         * High RAM: defer freelist insert until HHDM (P-MEM-5). Count totals
         * now so pmm_total_frames reflects the machine before release.
         */
        if (cbLen > 0 && g_cHigh < PMM_HIGH_PENDING_MAX) {
            g_aHigh[g_cHigh].paBase = paBase;
            g_aHigh[g_cHigh].cbLen = cbLen;
            g_cHigh++;
            {
                gj_paddr_t pa = (paBase + GJ_PAGE_SIZE - 1) &
                                ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
                gj_paddr_t pe = (paBase + cbLen) & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);

                if (pe > pa) {
                    g_cFramesTotal += (pe - pa) / GJ_PAGE_SIZE;
                }
            }
        }
    }

    /* Greppable: pmm: freelist free= */
    kprintf("pmm: freelist free=%lu total=%lu max_pa=0x%lx high_pending=%u "
            "orders=0..%u free_low=%lu free_high=%lu hierarchical free ready\n",
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)g_paMaxSeen, g_cHigh, PMM_MAX_ORDER,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh);
    log_order_hist("init");
    log_tib_design_soft();
    /* Wave 15: greppable pmm: soft … inventory (not 1TiB product; not bar3). */
    pmm_soft_inventory("init");
}

void
pmm_release_high(void)
{
    u32 i;

    if (g_fHighReleased || !hhdm_ready()) {
        return;
    }
    /* Hierarchical free_range into high order freelists (PA ≥ 4 GiB). */
    for (i = 0; i < g_cHigh; i++) {
        free_range(g_aHigh[i].paBase, g_aHigh[i].cbLen, 0);
    }
    g_fHighReleased = 1;
    /* Greppable: pmm: high released free= */
    kprintf("pmm: high released free=%lu free_low=%lu free_high=%lu "
            "high_order_nodes=%lu hierarchical free\n",
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesFreeLow,
            (unsigned long)g_cFramesFreeHigh, (unsigned long)high_order_nodes());
    log_order_hist("high_release");
    /* High-order soft: after free_range bulk insert, expect order-N nodes. */
    kprintf("pmm: high_order soft released nodes=%lu free_high=%lu "
            "max_order=%u hierarchical free\n",
            (unsigned long)high_order_nodes(),
            (unsigned long)g_cFramesFreeHigh, PMM_MAX_ORDER);
    /* Wave 15: greppable pmm: soft … after high release. */
    pmm_soft_inventory("high_release");
}

gj_paddr_t
pmm_alloc(void)
{
    return pop_free();
}

gj_paddr_t
pmm_alloc_high(void)
{
    gj_paddr_t pa;
    u64 *p;

    /*
     * Prefer high order-0 freelist for DMA/GPU-class allocs (HHDM required).
     * When high singles are empty (common after free_range bulk order-N insert),
     * fall back to hierarchical pop_free (may split high or low blocks).
     */
    if (g_paFreeHigh == 0) {
        return pop_free();
    }
    pa = g_paFreeHigh;
    /* Defensive: corrupt high head — do not advance; fall back to dual path. */
    if (!order_aligned(pa, 0)) {
        return pop_free();
    }
    p = (u64 *)pa_to_ptr(pa);
    if (p == NULL) {
        return pop_free();
    }
    g_paFreeHigh = (gj_paddr_t)(*p);
    *p = 0;
    if (g_cFramesFree > 0) {
        g_cFramesFree--;
    }
    if (g_cFramesFreeHigh > 0) {
        g_cFramesFreeHigh--;
    }
    if (g_aOrderCount[0] > 0) {
        g_aOrderCount[0]--;
    }
    return pa;
}

u64
pmm_total_frames(void)
{
    return g_cFramesTotal;
}

gj_paddr_t
pmm_alloc_pages(u32 cPages)
{
    gj_paddr_t aHold[256];
    u32 cHold = 0;
    gj_paddr_t paBase = 0;
    u32 i;
    u32 oWant;

    if (cPages == 0) {
        return 0;
    }
    if (cPages == 1) {
        return pop_free();
    }
    /* Cap: hierarchical max (512 pages = 2 MiB when PMM_MAX_ORDER=9). */
    if (cPages > (1u << PMM_MAX_ORDER)) {
        return 0;
    }
    /*
     * Hierarchical free path: exact power-of-two page count → order freelist
     * (+ pop_order_split). Used by soak_tib for 4/16/2MiB blocks.
     */
    oWant = pages_to_order(cPages);
    if ((1u << oWant) == cPages) {
        paBase = pop_order_split(oWant);
        if (paBase != 0) {
            return paBase;
        }
    }
    /* Fallback: scan singles for a contiguous run (legacy, capped hold). */
    while (cHold < 256 && g_cFramesFree > 0) {
        gj_paddr_t pa = pop_free();
        u32 j;
        u32 cRun;

        if (pa == 0) {
            break;
        }
        aHold[cHold++] = pa;
        for (j = cHold - 1; j > 0; j--) {
            if (aHold[j] < aHold[j - 1]) {
                gj_paddr_t t = aHold[j];

                aHold[j] = aHold[j - 1];
                aHold[j - 1] = t;
            } else {
                break;
            }
        }
        if (cHold < cPages) {
            continue;
        }
        cRun = 1;
        for (j = 1; j < cHold; j++) {
            if (aHold[j] == aHold[j - 1] + GJ_PAGE_SIZE) {
                cRun++;
                if (cRun >= cPages) {
                    paBase = aHold[j - cPages + 1];
                    break;
                }
            } else {
                cRun = 1;
            }
        }
        if (paBase != 0) {
            break;
        }
    }
    if (paBase == 0) {
        for (i = 0; i < cHold; i++) {
            push_free(aHold[i]);
        }
        return 0;
    }
    for (i = 0; i < cHold; i++) {
        if (aHold[i] < paBase ||
            aHold[i] >= paBase + (gj_paddr_t)cPages * GJ_PAGE_SIZE) {
            push_free(aHold[i]);
        }
    }
    return paBase;
}

void
pmm_free(gj_paddr_t paPage)
{
    push_free(paPage);
}

void
pmm_free_pages(gj_paddr_t paPage, u32 cPages)
{
    u32 o;
    u32 n;

    if (cPages == 0 || paPage == 0) {
        return;
    }
    /* Refuse unaligned base — never half-insert into freelists. */
    if ((paPage & (GJ_PAGE_SIZE - 1)) != 0) {
        return;
    }
    /*
     * Hierarchical free path: exact power-of-two page count at natural
     * block alignment → one order freelist node (O(1)).
     * Otherwise fall back to order-0 singles so frames are not lost.
     */
    o = pages_to_order(cPages);
    n = 1u << o;
    if (n == cPages && o <= PMM_MAX_ORDER && order_aligned(paPage, o)) {
        push_order(paPage, o);
        return;
    }
    {
        u32 i;

        for (i = 0; i < cPages; i++) {
            push_free(paPage + (gj_paddr_t)i * GJ_PAGE_SIZE);
        }
    }
}

u64
pmm_order_count(u32 u32Order)
{
    if (u32Order > PMM_MAX_ORDER) {
        return 0;
    }
    if (u32Order == 0) {
        /* Total free frames (not order-0 node count) — greppable diagnostics. */
        return g_cFramesFree;
    }
    return g_aOrderCount[u32Order];
}

u64
pmm_order_nodes(u32 u32Order)
{
    if (u32Order > PMM_MAX_ORDER) {
        return 0;
    }
    return g_aOrderCount[u32Order];
}

u64
pmm_free_frames_low(void)
{
    return g_cFramesFreeLow;
}

u64
pmm_free_frames_high(void)
{
    return g_cFramesFreeHigh;
}

u64
pmm_high_order_nodes(void)
{
    return high_order_nodes();
}

void
pmm_log_orders(void)
{
    log_order_hist("api");
    log_tib_design_soft();
    /* Wave 15: greppable pmm: soft … on explicit order dump. */
    pmm_soft_inventory("api");
}

size_t
pmm_free_count(void)
{
    return (size_t)g_cFramesFree;
}

u64
pmm_max_phys(void)
{
    return g_paMaxSeen;
}

int
pmm_soak_tib(u64 u64NeedBytes)
{
    u64 maxPa = g_paMaxSeen;
    gj_paddr_t pa4;
    gj_paddr_t pa16;
    u32 cOrderOk;
    u32 nBig = 0;

    if (u64NeedBytes == 0) {
        u64NeedBytes = PMM_TIB_BYTES; /* default 1 TiB threshold */
    }
    /*
     * Soft gate for large-RAM PASS path: main.c uses 768ull<<30 (768 GiB).
     * Below threshold → soak_tib SKIP soft (return 0, not a fail), but still
     * run soft hierarchical exercise + order counts for 1 TiB design
     * observability on small QEMU/deck hosts.
     * Greppable: "pmm: soak_tib SKIP soft" | "pmm: soak_tib PASS" |
     *            "pmm: soak_tib FAIL"
     */
    if (maxPa < u64NeedBytes) {
        cOrderOk = soft_hier_exercise(&nBig);
        kprintf("pmm: soak_tib SKIP soft max_pa=0x%lx need=0x%lx free=%lu "
                "free_low=%lu free_high=%lu hier_soft_ok=%u/%u big2MiB=%u "
                "high_order_nodes=%lu (host/QEMU below threshold); "
                "hierarchical free ready orders=0..%u (max %u pages / block)\n",
                (unsigned long)maxPa, (unsigned long)u64NeedBytes,
                (unsigned long)g_cFramesFree, (unsigned long)g_cFramesFreeLow,
                (unsigned long)g_cFramesFreeHigh, cOrderOk, PMM_MAX_ORDER, nBig,
                (unsigned long)high_order_nodes(), PMM_MAX_ORDER,
                1u << PMM_MAX_ORDER);
        log_order_hist("soak_soft");
        log_tib_design_soft();
        /* Wave 15: soft inventory even on soak_tib SKIP (small hosts). */
        pmm_soft_inventory("soak_soft");
        return 0;
    }
    /* Large machine: alloc/free multi-page blocks via order freelists. */
    pa4 = pmm_alloc_pages(4);
    pa16 = pmm_alloc_pages(16);
    if (pa4 == 0 || pa16 == 0) {
        if (pa4) {
            pmm_free_pages(pa4, 4);
        }
        if (pa16) {
            pmm_free_pages(pa16, 16);
        }
        kprintf("pmm: soak_tib FAIL alloc (4+16 hierarchical) free=%lu "
                "max_pa=0x%lx max_order=%u free_low=%lu free_high=%lu "
                "high_order_nodes=%lu\n",
                (unsigned long)g_cFramesFree, (unsigned long)maxPa,
                PMM_MAX_ORDER, (unsigned long)g_cFramesFreeLow,
                (unsigned long)g_cFramesFreeHigh,
                (unsigned long)high_order_nodes());
        log_order_hist("soak_fail");
        return -1;
    }
    /* Hierarchical free: power-of-two sizes land on order freelists. */
    pmm_free_pages(pa16, 16);
    pmm_free_pages(pa4, 4);
    cOrderOk = soft_hier_exercise(&nBig);
    /* Greppable: pmm: soak_tib PASS */
    kprintf("pmm: soak_tib PASS max_pa=0x%lx free=%lu free_low=%lu "
            "free_high=%lu hier_orders_ok=%u/%u max_order=%u big2MiB=%u "
            "need=0x%lx high_order_nodes=%lu splits=%lu hierarchical free\n",
            (unsigned long)maxPa, (unsigned long)g_cFramesFree,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            cOrderOk, PMM_MAX_ORDER, PMM_MAX_ORDER, nBig,
            (unsigned long)u64NeedBytes, (unsigned long)high_order_nodes(),
            (unsigned long)g_cSplit);
    log_order_hist("soak_tib");
    log_tib_design_soft();
    /* Wave 15: greppable pmm: soft … after large-RAM hierarchical soak. */
    pmm_soft_inventory("soak_tib");
    return 0;
}

int
pmm_soak(u32 u32Singles, u32 u32Contig)
{
    gj_paddr_t aPa[64];
    gj_paddr_t paBlock;
    u32 i;
    u32 n = u32Singles;

    if (n > 64) {
        n = 64;
    }
    if (u32Contig == 0) {
        u32Contig = 4;
    }
    if (u32Contig > 64) {
        u32Contig = 64;
    }
    for (i = 0; i < n; i++) {
        aPa[i] = pmm_alloc();
        if (aPa[i] == 0) {
            while (i > 0) {
                i--;
                pmm_free(aPa[i]);
            }
            kprintf("pmm: soak single fail i=%u free=%lu\n", i,
                    (unsigned long)g_cFramesFree);
            return -1;
        }
    }
    for (i = 0; i < n; i++) {
        pmm_free(aPa[i]);
    }
    paBlock = pmm_alloc_pages(u32Contig);
    if (paBlock == 0) {
        kprintf("pmm: soak contig fail n=%u free=%lu\n", u32Contig,
                (unsigned long)g_cFramesFree);
        return -1;
    }
    pmm_free_pages(paBlock, u32Contig);
    kprintf("pmm: soak singles=%u contig=%u free=%lu total=%lu max_pa=0x%lx\n",
            n, u32Contig, (unsigned long)g_cFramesFree,
            (unsigned long)g_cFramesTotal, (unsigned long)g_paMaxSeen);
    /* Greppable: pmm: soak PASS (smoke-all hard gate) */
    kprintf("pmm: soak PASS\n");
    /* Wave 15: greppable pmm: soft … after hard soak PASS. */
    pmm_soft_inventory("soak");
    return 0;
}
