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
 * Serial markers (scripts/gj-soak-large-ram.sh, smoke-all, product-summary):
 *   "pmm: freelist free="
 *   "pmm: high released free="
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
static u64        g_cFramesTotal;
static u64        g_paMaxSeen;
static gj_paddr_t g_paKernel0;
static gj_paddr_t g_paKernel1;
static struct pmm_pending g_aHigh[PMM_HIGH_PENDING_MAX];
static u32        g_cHigh;
static int        g_fHighReleased;

static gj_paddr_t pop_order_split(u32 u32Order);

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
    g_aOrderCount[u32Order]++;
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
        while (o > u32Order) {
            o--;
            push_order(pa + ((gj_paddr_t)(1u << o) * GJ_PAGE_SIZE), o);
        }
        return pa;
    }
    return 0;
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
    g_cFramesTotal = 0;
    g_paMaxSeen = 0;
    g_cHigh = 0;
    g_fHighReleased = 0;
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
            "orders=0..%u hierarchical free ready\n",
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)g_paMaxSeen, g_cHigh, PMM_MAX_ORDER);
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
    kprintf("pmm: high released free=%lu\n", (unsigned long)g_cFramesFree);
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
    u32 o;
    u32 cOrderOk;

    if (u64NeedBytes == 0) {
        u64NeedBytes = 1ull << 40; /* default 1 TiB threshold */
    }
    /*
     * Soft gate: only large-RAM hosts run the hierarchical exercise.
     * main.c uses 768ull<<30 (768 GiB class); deck/small QEMU soft-SKIP
     * with return 0 — not a soak failure.
     * Greppable: "pmm: soak_tib SKIP soft" | "pmm: soak_tib PASS" |
     *            "pmm: soak_tib FAIL"
     * Hierarchical free (orders 0..PMM_MAX_ORDER) remains available for
     * power-of-two multi-page alloc/free on any RAM size.
     */
    if (maxPa < u64NeedBytes) {
        kprintf("pmm: soak_tib SKIP soft max_pa=0x%lx need=0x%lx free=%lu "
                "(host/QEMU below threshold); hierarchical free ready "
                "orders=0..%u (max %u pages / block)\n",
                (unsigned long)maxPa, (unsigned long)u64NeedBytes,
                (unsigned long)g_cFramesFree, PMM_MAX_ORDER,
                1u << PMM_MAX_ORDER);
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
                "max_pa=0x%lx max_order=%u\n",
                (unsigned long)g_cFramesFree, (unsigned long)maxPa,
                PMM_MAX_ORDER);
        return -1;
    }
    /* Hierarchical free: power-of-two sizes land on order freelists. */
    pmm_free_pages(pa16, 16);
    pmm_free_pages(pa4, 4);
    cOrderOk = 0;
    for (o = 1; o <= PMM_MAX_ORDER; o++) {
        gj_paddr_t pa = pmm_alloc_pages(1u << o);

        if (pa != 0) {
            pmm_free_pages(pa, 1u << o);
            cOrderOk++;
        }
    }
    /* Extra: a few max-order (2 MiB) blocks when free is huge (768G path). */
    {
        u32 nBig = 0;
        u32 i;

        for (i = 0; i < 8u; i++) {
            gj_paddr_t pa = pmm_alloc_pages(1u << PMM_MAX_ORDER);

            if (pa == 0) {
                break;
            }
            pmm_free_pages(pa, 1u << PMM_MAX_ORDER);
            nBig++;
        }
        /* Greppable: pmm: soak_tib PASS */
        kprintf("pmm: soak_tib PASS max_pa=0x%lx free=%lu "
                "hier_orders_ok=%u/%u max_order=%u big2MiB=%u need=0x%lx "
                "hierarchical free\n",
                (unsigned long)maxPa, (unsigned long)g_cFramesFree, cOrderOk,
                PMM_MAX_ORDER, PMM_MAX_ORDER, nBig,
                (unsigned long)u64NeedBytes);
    }
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
    return 0;
}
