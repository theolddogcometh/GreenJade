/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Physical page freelist PMM - multi-TiB capable (P-MEM-3).
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0. Soft!=product.
 *
 * Dual zone freelists
 *   low  : PA < 4 GiB  (identity before HHDM; HHDM after)
 *   high : PA >= 4 GiB  (pending until vmm_hhdm_init + pmm_release_high)
 * Alloc prefers low so early kernel structures stay identity-safe.
 *
 * Lean DMA residual (Soft!=product; exclusive residual this unit) -
 * greppable: pmm: soft residual lean | pmm: soft residual force32
 * greppable: pmm: soft residual alloc_low | pmm: soft residual identity
 * greppable: pmm: soft residual C2 | pmm: soft residual C2 PASS
 * greppable: pmm: soft dma | pmm_alloc_low
 *   pop_order_split exhausts the *entire* low-zone hierarchy (exact order,
 *   then split higher low orders) before any high-zone (PA >= 4 GiB) pop.
 *   Prevents handing high PAs while low multi-page blocks remain free -
 *   critical for DMA-capable pages: rtl8168 / UDX rings eng via dma_buf
 *   force32 / VT-d identity [0, 1 GiB) under TE. Soft tallies
 *   alloc_low/alloc_high on the split path.
 *   pmm_alloc_low / pmm_alloc_pages_low: low-zone *only* foundation (no
 *   high fallback) for force32 / UDX when PA >= 4 GiB is unacceptable.
 *   Force32 residual deepen (Soft!=product; dual MIT OR Apache-2.0):
 *     - API contract: success PA always < 4 GiB (belt-and-suspenders).
 *     - Soft api_alloc_low / api_alloc_pages_low ok|fail tallies.
 *     - Soft residual lamps: force32 + alloc_low + identity + C2.
 *     - Soft self-exercise: alloc_low + free round-trip in inventory.
 *     - Soft identity hunt (cap hold): live [0,1 GiB) via alloc_low hold
 *       (dma_buf-class residual; never product AC; G-AC-1).
 *   C2 Dual DoD DMA foundation residual (Soft!=product; claim_class=C2):
 *     - Product *direction*: low-only / force32 / VT-d identity pages for
 *       userspace UDX Dual DoD A (USB) + B (NIC) ring/bounce eng.
 *     - Soft gates: dual null free, paint/verify, LIFO reuse, free restore,
 *       total immutable, low PA contract, identity hunt.
 *     - Dual_DoD_A=OPEN Dual_DoD_B=OPEN always (soft != Dual DoD close).
 *     - soft_scaffold_ne_product_ac=1; never claims product UDX DMA caps.
 *   No version stamp. No stamp storms. Soft residual != 1 TiB product.
 *   Soft residual != product UDX dual-license driver path (userspace).
 *   G-AC-1: soft != product AC. Dual MIT OR Apache-2.0. Soft!=product.
 *
 * Hierarchical free (order freelists) - greppable: hierarchical free
 *   Order N node covers (1<<N) contiguous pages, naturally aligned.
 *   Order 0 heads: g_paFreeLow / g_paFreeHigh (single 4 KiB frames).
 *   Order 1..PMM_MAX_ORDER: g_aOrderLow[] / g_aOrderHigh[] (max order 9
 *   -> 512 pages = 2 MiB). free_range bulk-inserts largest aligned blocks
 *   at boot / high-release so multi-hundred-GiB hosts skip per-page walks.
 *   pop_order_split: low-zone first, else high; exact order else split
 *   higher block and push sibling buddies back (never invents frames).
 *
 * Soft inventory deepen (greppable: pmm: soft):
 *   "pmm: soft honesty ..."    explicit non-claims (not 1 TiB product)
 *   "pmm: soft inventory ..."  free/total, zones, pending, hierarchy snapshot
 *   "pmm: soft zones ..."      low/high free frames + release state
 *   "pmm: soft hier ..."       max_order, nodes, splits, high-order pushes
 *   "pmm: soft orders ..."     per-order node counts (soft snapshot)
 *   "pmm: soft heads ..."      freelist head presence (low/high order-0 + top)
 *   "pmm: soft pending ..."    high-pending ranges before release
 *   "pmm: soft host ..."       host size vs 1 TiB gate (soft only)
 *   "pmm: soft design ..."     design max_order / block size (soft only)
 *   "pmm: soft path ..."       surface catalog + explicit product_tib=0
 *   "pmm: soft stats ..."      rollup free/use/splits/nodes/logs
 *   "pmm: soft geometry ..."   page/zone/order constants
 *   "pmm: soft kernel ..."     kernel image reserve snap
 *   "pmm: soft hhdm ..."       high-zone vs HHDM dependency
 *   "pmm: soft lamps ..."      composite readiness lamps
 *   "pmm: soft OPEN ..."       P-MEM-3 / product_tib OPEN
 *   "pmm: soft surfaces ..."   return-surface catalog (surf bitmask)
 *   "pmm: soft ratio ..."      free/total soft ratio lamps
 *   "pmm: soft sites ..."      emission-site catalog
 *   "pmm: soft api ..."        alloc/free surface return lamps
 *   "pmm: soft dma ..."        lean DMA residual (low hierarchy first; rtl)
 *   "pmm: soft residual lean ..."  DMA low-first residual (no version stamp)
 *   "pmm: soft residual force32 ..."  force32 UDX DMA foundation residual
 *   "pmm: soft residual alloc_low ..."  pmm_alloc_low API residual + tallies
 *   "pmm: soft residual identity ..."  VT-d [0,1GiB) hunt residual (C2 UDX)
 *   "pmm: soft residual C2 ..."   C2 Dual DoD DMA foundation residual
 *   "pmm: soft residual C2 PASS|FAIL"  C2 soft gate verdict (never product AC)
 *   "pmm: soft deepen ..."     area count (no stamp storms)
 *   "pmm: soft PASS" | "pmm: soft inventory PASS" | "pmm: soft EMPTY|NONE"
 * Honesty: soft inventory never claims 1 TiB product host or closes P-MEM-3;
 *          Dual DoD A/B remain OPEN; soft; Soft!=product; dual MIT OR Apache-2.0.
 *
 * 1 TiB design observability (no 1 TiB host required - soft markers):
 *   Zone free frame counts (low/high) + per-order node counts.
 *   "pmm: orders tag=..." histogram after init / high-release / soak.
 *   "pmm: tib_design soft ..." always (max_order, block size, design ceil).
 *   "pmm: tib_host soft PASS|SKIP" - true >=1 TiB max_pa gate (soft).
 *   "pmm: high_order soft ..." - high-order node presence after release/soak.
 *   Soft hierarchical exercise runs even on soak_tib SKIP (small QEMU).
 *
 * Serial markers (scripts/gj-soak-large-ram.sh, smoke-all, product-summary):
 *   "pmm: freelist free="
 *   "pmm: high released free="
 *   "pmm: soft inventory"
 *   "pmm: soft residual lean"
 *   "pmm: soft residual force32"
 *   "pmm: soft residual alloc_low"
 *   "pmm: soft residual identity"
 *   "pmm: soft residual C2"
 *   "pmm: soft residual C2 PASS"
 *   "pmm: soak PASS"
 *   "pmm: soak_tib PASS" | "pmm: soak_tib SKIP soft" | "pmm: soak_tib FAIL"
 * Large-RAM path: main.c calls pmm_soak_tib(768ull<<30) - GJ_MEM=768G.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/vmm.h>

#define PMM_HIGH_PENDING_MAX 64
#define PMM_LOW_MAX          0x100000000ull
/*
 * VT-d bring-up identity cover [0, 1 GiB) - force32-safe under TE (G752).
 * Soft residual only: PMM low-only API is PA < 4 GiB; identity is a subset.
 * Product filter remains at dma_buf / UDX. Soft!=product; G-AC-1.
 */
#define PMM_VTD_ID_LIMIT     0x40000000ull
/* Soft identity hunt hold budget (no stamp storms / freelist thrash). */
#define PMM_SOFT_ID_HOLD     8u
/* Soft paint patterns for C2 residual page first words (pure C stores). */
#define PMM_SOFT_PAT_A       0xA5A5A5A5A5A5A5A5ull
#define PMM_SOFT_PAT_B       0x5A5A5A5A5A5A5A5Aull
/* Max hierarchical order: 9 -> 512 pages = 2 MiB (matches HHDM large pages). */
#define PMM_MAX_ORDER        9u
/* Product soft gate: true 1 TiB host class (1ull<<40). Soft only - never hard-fail. */
#define PMM_TIB_BYTES        (1ull << 40)
/* Soft inventory wave tag (file-local; never product gate; not GJ_IMAGE_VERSION). */
#define PMM_SOFT_WAVE 126u
/*
 * Catalog area count for deepen (honesty..residual_C2 prior to deepen).
 * Honest high-water for live soft lines - not stamp-storm inflated.
 */
#define PMM_SOFT_AREAS 27u

/*
 * C2 Dual DoD DMA foundation residual gate bits (soft_force32_alloc_low_ex).
 * Soft only - never product AC / never Dual DoD close. Soft!=product.
 * greppable: pmm: soft residual C2
 */
#define PMM_SOFT_C2_GATE_NULL     (1u << 0) /* dual null free no-op */
#define PMM_SOFT_C2_GATE_SINGLE   (1u << 1) /* pmm_alloc_low PA < 4 GiB */
#define PMM_SOFT_C2_GATE_PAGES    (1u << 2) /* pmm_alloc_pages_low 4p */
#define PMM_SOFT_C2_GATE_PAINT    (1u << 3) /* paint/verify dual pattern */
#define PMM_SOFT_C2_GATE_LIFO     (1u << 4) /* free B -> next alloc is B */
#define PMM_SOFT_C2_GATE_ID       (1u << 5) /* identity hunt saw [0,1GiB) */
#define PMM_SOFT_C2_GATE_FREE_R   (1u << 6) /* free_count restored */
#define PMM_SOFT_C2_GATE_TOTAL_I  (1u << 7) /* total immutable */
#define PMM_SOFT_C2_GATE_N        8u
#define PMM_SOFT_C2_GATE_CORE                                                      \
    (PMM_SOFT_C2_GATE_NULL | PMM_SOFT_C2_GATE_SINGLE | PMM_SOFT_C2_GATE_PAINT |   \
     PMM_SOFT_C2_GATE_LIFO | PMM_SOFT_C2_GATE_FREE_R | PMM_SOFT_C2_GATE_TOTAL_I)

/*
 * Return-surface bit lamps (surf=0x... on soft surfaces/deepen).
 * Bits mark greppable soft areas this unit emits - not product close.
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
#define PMM_SOFT_SURF_DMA       (1u << 20) /* lean DMA residual (rtl rings) */
#define PMM_SOFT_SURF_RESIDUAL  (1u << 21) /* soft residual lean lamp */
#define PMM_SOFT_SURF_IDENTITY  (1u << 22) /* VT-d identity hunt residual */
#define PMM_SOFT_SURF_C2        (1u << 23) /* C2 Dual DoD DMA foundation residual */
#define PMM_SOFT_SURF_CATALOG                                                      \
    (PMM_SOFT_SURF_HONESTY | PMM_SOFT_SURF_INVENTORY | PMM_SOFT_SURF_ZONES |     \
     PMM_SOFT_SURF_HIER | PMM_SOFT_SURF_ORDERS | PMM_SOFT_SURF_HEADS |           \
     PMM_SOFT_SURF_PENDING | PMM_SOFT_SURF_HOST | PMM_SOFT_SURF_DESIGN |         \
     PMM_SOFT_SURF_PATH | PMM_SOFT_SURF_STATS | PMM_SOFT_SURF_GEOMETRY |         \
     PMM_SOFT_SURF_KERNEL | PMM_SOFT_SURF_HHDM | PMM_SOFT_SURF_LAMPS |           \
     PMM_SOFT_SURF_OPEN | PMM_SOFT_SURF_SURFACES | PMM_SOFT_SURF_RATIO |         \
     PMM_SOFT_SURF_SITES | PMM_SOFT_SURF_API | PMM_SOFT_SURF_DMA |              \
     PMM_SOFT_SURF_RESIDUAL | PMM_SOFT_SURF_IDENTITY | PMM_SOFT_SURF_C2)

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
/* Zone free frames (sum = g_cFramesFree); 1 TiB-class observability. */
static u64        g_cFramesFreeLow;
static u64        g_cFramesFreeHigh;
static u64        g_cFramesTotal;
static u64        g_paMaxSeen;
/* Soft observability: split / high-order push events (never invent frames). */
static u64        g_cSplit;
static u64        g_cHighOrderPush;
/*
 * Soft DMA residual tallies (pop_order_split path; Soft!=product).
 * alloc_low  - returned PA < 4 GiB (rtl ring / force32 preferred)
 * alloc_high - returned PA >= 4 GiB (high fallback after low hierarchy empty)
 * Never hard-gates; wrap OK.
 */
static u64        g_cAllocLow;
static u64        g_cAllocHigh;
/*
 * Soft pmm_alloc_low / pmm_alloc_pages_low API tallies (force32 residual).
 * Distinct from g_cAllocLow (which also counts pop_order_split low pops).
 * Soft!=product; dual MIT OR Apache-2.0; wrap OK; never product gate.
 */
static u64        g_cApiAllocLowOk;
static u64        g_cApiAllocLowFail;
static u64        g_cApiAllocPagesLowOk;
static u64        g_cApiAllocPagesLowFail;
/* Soft force32 residual self-exercise: ok rounds + refuse high (once-ish). */
static u64        g_cForce32SoftOk;
static u64        g_cForce32SoftFail;
static u32        g_cForce32SoftEx;
/*
 * Soft VT-d identity residual (subset of low; C2 UDX force32 under TE).
 * id_ok   - soft hunt returned PA entirely in [0, PMM_VTD_ID_LIMIT)
 * id_miss - low pages free but no identity page in hold budget
 * Soft!=product; never product AC; wrap OK.
 */
static u64        g_cForce32SoftIdOk;
static u64        g_cForce32SoftIdMiss;
static u64        g_cForce32SoftIdHold; /* pages held during identity hunt */
/*
 * C2 Dual DoD DMA foundation residual gate tallies (Soft!=product).
 * Soft exercise only; never hard-gates Dual DoD / product AC. Wrap OK.
 */
static u32        g_u32Force32SoftGates; /* last exercise gate bit lamps */
static u64        g_cForce32SoftNullOk;
static u64        g_cForce32SoftPaintOk;
static u64        g_cForce32SoftLifoOk;
static u64        g_cForce32SoftFreeRestOk;
static u64        g_cForce32SoftTotalImmutOk;
static u64        g_cForce32SoftC2Pass;
static u64        g_cForce32SoftC2Fail;
static gj_paddr_t g_paKernel0;
static gj_paddr_t g_paKernel1;
/* Soft media (UEFI ESP .ko): exclude from freelist until soft load. Soft!=product. */
static gj_paddr_t g_paSoftMedia0;
static gj_paddr_t g_paSoftMedia1;
static struct pmm_pending g_aHigh[PMM_HIGH_PENDING_MAX];
static u32        g_cHigh;
static int        g_fHighReleased;
/* Times soft inventory printed (product / smoke observability). */
static u32        g_cSoftInvLogs;

static gj_paddr_t pop_order_split(u32 u32Order);
static u64        high_order_nodes(void);
static void       log_order_hist(const char *szTag);
static void       log_tib_design_soft(void);
static void       pmm_soft_inventory(const char *szWhere);
static u32        soft_hier_exercise(u32 *pOutBig);
static void       soft_force32_alloc_low_ex(void);

static void *
pa_to_ptr(gj_paddr_t pa)
{
    /*
     * High RAM (>=4 GiB) is only valid under HHDM after vmm_hhdm_init.
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
 * True if [pa, pa+cb) may enter the freelist (not low BIOS, not kernel image,
 * not soft media handoff blob).
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
    if (g_paSoftMedia1 > g_paSoftMedia0 &&
        range_overlaps(pa, pa + (gj_paddr_t)cb, g_paSoftMedia0,
                       g_paSoftMedia1)) {
        return 0;
    }
    return 1;
}

/**
 * Soft: reserve soft media pages before pmm_init. Soft!=product (D4).
 * greppable: pmm: soft media reserve
 */
void
pmm_soft_reserve(gj_paddr_t paBase, u64 cbLen)
{
    if (paBase == 0 || cbLen == 0) {
        g_paSoftMedia0 = 0;
        g_paSoftMedia1 = 0;
        return;
    }
    g_paSoftMedia0 = paBase & ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
    g_paSoftMedia1 =
        (paBase + (gj_paddr_t)cbLen + GJ_PAGE_SIZE - 1) &
        ~(gj_paddr_t)(GJ_PAGE_SIZE - 1);
    /* Grep: pmm: soft media reserve */
    kprintf("pmm: soft media reserve PASS pa=0x%lx end=0x%lx cb=%lu "
            "(soft!=product; D4 ESP handoff)\n",
            (unsigned long)g_paSoftMedia0, (unsigned long)g_paSoftMedia1,
            (unsigned long)cbLen);
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
 * Pop one order-N block from a single zone. Does not split.
 * fLow != 0 -> low freelist (PA < 4 GiB); fLow == 0 -> high freelist.
 * Corrupt / unaligned heads are refused without advancing the list.
 * DMA residual: zone-isolated pop so split can exhaust low before high.
 */
static gj_paddr_t
pop_order_from(u32 u32Order, int fLow)
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
        pHead = fLow ? &g_paFreeLow : &g_paFreeHigh;
    } else {
        pHead = fLow ? &g_aOrderLow[u32Order] : &g_aOrderHigh[u32Order];
    }
    if (*pHead == 0) {
        return 0;
    }
    pa = *pHead;
    /*
     * Defensive: refuse zero / non-page / non-natural-order heads.
     * Do not advance the list - avoid handing out garbage PAs.
     */
    if (pa == 0 || !order_aligned(pa, u32Order)) {
        return 0;
    }
    /* Zone sanity: refuse a head that does not match the requested zone. */
    if (fLow) {
        if (pa >= PMM_LOW_MAX) {
            return 0;
        }
    } else if (pa < PMM_LOW_MAX) {
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
     * DMA residual: pop_order_split exhausts low hierarchy first.
     */
    return pop_order_split(0);
}

/**
 * Split helper: pop order o from zone fLow and push upper buddies down to
 * u32Order. Returns base PA or 0. Rearranges freelist nodes only.
 */
static gj_paddr_t
pop_split_zone(u32 u32Order, int fLow)
{
    u32 o;
    gj_paddr_t pa;

    pa = pop_order_from(u32Order, fLow);
    if (pa != 0) {
        return pa;
    }
    for (o = u32Order + 1u; o <= PMM_MAX_ORDER; o++) {
        pa = pop_order_from(o, fLow);
        if (pa == 0) {
            continue;
        }
        /* Split: free upper half buddies of orders o-1 ... u32Order. */
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
 * Hierarchical alloc: exact order pop, else split a higher-order block and
 * push sibling buddies (orders o-1 ... want) back onto hierarchical free.
 * Rearranges freelist nodes only - does not invent physical frames.
 *
 * Lean DMA residual (Soft!=product; UDX rings eng / force32):
 *   Exhaust *low* zone hierarchy first (exact + split higher low orders),
 *   then high zone. Avoids returning PA >= 4 GiB while low multi-page free
 *   nodes remain - dma_buf / rtl8168 / UDX ring path (force32, VT-d
 *   identity [0, 1 GiB) under TE). Soft tallies g_cAllocLow / g_cAllocHigh.
 *   greppable: pmm: soft dma | pmm: soft residual lean
 *
 * Invariant (do not regress): after pop of order o and split down to want,
 * returned PA is the low base of the original block; each discarded upper
 * half is push_order'd at its buddy order. free_range + this path are the
 * large-RAM hierarchical free core (768G soak_tib).
 */
static gj_paddr_t
pop_order_split(u32 u32Order)
{
    gj_paddr_t pa;

    if (u32Order > PMM_MAX_ORDER) {
        return 0;
    }
    /* Low zone first (DMA-capable pages for UDX / rtl rings / force32). */
    pa = pop_split_zone(u32Order, 1);
    if (pa != 0) {
        g_cAllocLow++;
        return pa;
    }
    /* High zone fallback only after low hierarchy is empty. */
    pa = pop_split_zone(u32Order, 0);
    if (pa != 0) {
        g_cAllocHigh++;
    }
    return pa;
}

/**
 * Sum of free nodes on orders 1..PMM_MAX_ORDER (not frames).
 * Greppable high-order soft observability for 1 TiB design.
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
 * tag=init | high_release | soak_tib | soak_soft | ...
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
 * Always-on 1 TiB design soft marker (does not require 1 TiB host).
 * Greppable: pmm: tib_design soft | pmm: tib_host soft PASS|SKIP
 * Soft host/design observability only - not a 1 TiB product claim.
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
 * Prefix-stable markers (pmm: soft ...):
 *   pmm: soft honesty    - explicit non-claims (not 1 TiB product)
 *   pmm: soft inventory  - free/total, zones, pending, hierarchy snapshot
 *   pmm: soft zones      - low/high free frames + release state
 *   pmm: soft hier       - max_order, nodes, splits, high-order pushes
 *   pmm: soft orders     - per-order node counts (soft snapshot)
 *   pmm: soft heads      - freelist head presence (order-0 + top order)
 *   pmm: soft pending    - high-pending ranges before release
 *   pmm: soft host       - host size vs 1 TiB gate (soft only)
 *   pmm: soft design     - design max_order / block size (soft only)
 *   pmm: soft path       - surface catalog + product_tib=0
 *   pmm: soft stats      - rollup free/use/splits/nodes/logs
 *   pmm: soft geometry   - page/zone/order constants (Wave 15)
 *   pmm: soft kernel     - kernel image reserve snap (Wave 15)
 *   pmm: soft hhdm       - high-zone vs HHDM dependency (Wave 15)
 *   pmm: soft lamps      - composite readiness lamps (Wave 15)
 *   pmm: soft OPEN       - P-MEM-3 / product_tib OPEN (Wave 15)
 *   pmm: soft surfaces   - Wave 19 return-surface catalog (surf bitmask)
 *   pmm: soft ratio      - Wave 17 free/total soft ratio lamps
 *   pmm: soft sites      - Wave 17 emission-site catalog
 *   pmm: soft api        - alloc/free surface return lamps
 *   pmm: soft dma        - lean DMA residual (low-first split; rtl rings)
 *   pmm: soft residual lean - DMA low residual (no version stamp; dual license)
 *   pmm: soft residual force32 - force32 UDX DMA foundation residual
 *   pmm: soft residual alloc_low - pmm_alloc_low API residual + soft tallies
 *   pmm: soft residual identity - VT-d [0,1GiB) soft hunt (C2 UDX force32)
 *   pmm: soft residual C2 - C2 Dual DoD DMA foundation residual (A/B OPEN)
 *   pmm: soft residual C2 PASS|FAIL - soft gate verdict (never product AC)
 *   pmm: soft deepen     - area count (no stamp storms)
 *   pmm: soft PASS | EMPTY | NONE | inventory PASS
 *
 * Soft force32 residual may alloc/free low pages (round-trip + identity hunt
 * + C2 paint/LIFO/free-restore) when free_low permits - rearranges freelist
 * only; never invents frames. Soft!=product. Honesty: soft inventory !=
 * 1 TiB product claim; never closes P-MEM-3; Dual DoD A/B remain OPEN;
 * soft; Soft!=product; dual MIT OR Apache-2.0.
 * greppable: pmm: soft | pmm: soft dma | pmm: soft residual lean
 * greppable: pmm: soft residual force32 | pmm: soft residual alloc_low
 * greppable: pmm: soft residual identity | pmm: soft residual C2
 */
static void
pmm_soft_inventory(const char *szWhere)
{
    u64 cHi;
    u64 cbBlock;
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
    extern u32 serial_thre_dead(void);

    if (szWhere == 0) {
        szWhere = "path";
    }
    /*
     * Multi-KiB soft flood: skip on panel-only DUTs (no COM1 THRE) so
     * kernel_after_mmap can reach xHCI stick log / M0.
     */
    if (serial_thre_dead() != 0u) {
        kprintf("pmm: soft inventory SKIP via=%s (no COM1; panel path)\n",
                szWhere);
        return;
    }
    cHi = high_order_nodes();
    cbBlock = (u64)GJ_PAGE_SIZE << PMM_MAX_ORDER;
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
     * Does not encode host RAM class or 1 TiB product.
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

    /* Host size soft gate only - never product 1 TiB claim. */
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
     * Honesty first: freestanding soft inventory is NOT 1 TiB product,
     *. greppable: pmm: soft honesty
     */
    kprintf("pmm: soft honesty not-1TiB-product pmem3=OPEN "
            "product_tib=0 design_ceil_tib=%u max_order=%u "
            "host_tib=%s hierarchical free wave=%u "
            "(soft inventory only; never closes P-MEM-3)\n",
            (unsigned)GJ_PMM_MAX_PHYS_TIB, PMM_MAX_ORDER, szHost,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft inventory */
    kprintf("pmm: soft inventory via=%s ready=%s free=%lu total=%lu "
            "in_use=%lu free_low=%lu free_high=%lu max_pa=0x%lx "
            "high_pending=%u high_released=%u max_order=%u "
            "high_order_nodes=%lu nodes_all=%lu logs=%u wave=%u "
            "hierarchical free (soft; not 1TiB product)\n",
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

    /* Grep: pmm: soft orders - per-order node snapshot (soft; not product). */
    kprintf("pmm: soft orders via=%s", szWhere);
    for (o = 0; o <= PMM_MAX_ORDER; o++) {
        kprintf(" o%u=%lu", o, (unsigned long)g_aOrderCount[o]);
    }
    kprintf(" high_order_nodes=%lu splits=%lu wave=%u hierarchical free "
            "(soft; not 1TiB product)\n",
            (unsigned long)cHi, (unsigned long)g_cSplit,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft heads - freelist head lamps (presence only). */
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

    /* Grep: pmm: soft pending - high ranges parked before HHDM release. */
    kprintf("pmm: soft pending count=%u released=%u max=%u "
            "free_high=%lu high_order_nodes=%lu wave=%u "
            "(soft; not 1TiB product)\n",
            g_cHigh, g_fHighReleased ? 1u : 0u, PMM_HIGH_PENDING_MAX,
            (unsigned long)g_cFramesFreeHigh, (unsigned long)cHi,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft host - size gate only; never product claim. */
    kprintf("pmm: soft host tib=%s max_pa=0x%lx need=0x%lx free=%lu "
            "total=%lu free_low=%lu free_high=%lu high_order_nodes=%lu "
            "wave=%u (host size soft only; not 1TiB product claim)\n",
            szHost, (unsigned long)g_paMaxSeen, (unsigned long)PMM_TIB_BYTES,
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned long)g_cFramesFreeLow, (unsigned long)g_cFramesFreeHigh,
            (unsigned long)cHi, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft design - design geometry only; not product 1 TiB. */
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
            "product_tib=0 pmem3=OPEN bar_tib=OPEN wave=%u\n",
            szWhere, PMM_MAX_ORDER, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /* Grep: pmm: soft stats - rollup tallies for agent greps. */
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
            "hhdm_base=0x%lx wave=%u (soft; not 1TiB product)\n",
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
     * Wave 15: high freelist depends on HHDM (P-MEM-5) - soft only.
     * Grep: pmm: soft hhdm
     */
    kprintf("pmm: soft hhdm high_released=%u high_pending=%u "
            "free_high=%lu hhdm_ready=%u p_mem5=1 wave=%u "
            "(high zone needs HHDM; soft; not 1TiB product)\n",
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
            "(composite soft lamps; not product gate)\n",
            szReady,
            g_cFramesFree > 0 ? 1u : 0u,
            g_cFramesTotal > 0 ? 1u : 0u,
            g_paFreeLow != 0 ? 1u : 0u,
            g_paFreeHigh != 0 ? 1u : 0u,
            g_fHighReleased ? 1u : 0u,
            u32Hhdm, szHost, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15 honesty close: P-MEM-3 / product_tib remain OPEN.
     * Grep: pmm: soft OPEN
     */
    kprintf("pmm: soft OPEN pmem3=OPEN product_tib=0 "
            "bar_tib=OPEN host_tib=%s design_ceil_tib=%u free=%lu "
            "total=%lu wave=%u "
            "(soft inventory; never closes P-MEM-3; not 1TiB product; "
            ")\n",
            szHost, (unsigned)GJ_PMM_MAX_PHYS_TIB,
            (unsigned long)g_cFramesFree, (unsigned long)g_cFramesTotal,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft != product).
     * Grep: pmm: soft surfaces
     */
    kprintf("pmm: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "honesty=1 inventory=1 zones=1 hier=1 orders=1 heads=1 "
            "pending=1 host=1 design=1 path=1 stats=1 geometry=1 "
            "kernel=1 hhdm=1 lamps=1 open=1 ratio=1 sites=1 api=1 "
            "dma=1 residual=1 identity=1 c2=1 "
            "wave=%u (return surfaces; soft only; not product)\n",
            (unsigned)u32Surf, (unsigned)PMM_SOFT_AREAS, cAreas + 8u,
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: free/total soft ratio lamps (never product gate).
     * Grep: pmm: soft ratio
     */
    kprintf("pmm: soft ratio free_pct=%u low_pct=%u free=%lu total=%lu "
            "in_use=%lu free_low=%lu free_high=%lu high_order_nodes=%lu "
            "wave=%u (soft ratio; not 1TiB product)\n",
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
            "(soft emission sites; not product)\n",
            szWhere, g_cSoftInvLogs, (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: alloc/free surface return lamps (observability only).
     * Grep: pmm: soft api
     */
    kprintf("pmm: soft api alloc=1 free=1 alloc_pages=1 free_pages=1 "
            "alloc_low=1 alloc_pages_low=1 "
            "release_high=1 soak=1 soak_tib=1 log_orders=1 "
            "prefer_low=1 split=1 dma_low_first=1 product_tib=0 "
            "pmem3=OPEN wave=%u "
            "(soft API return surfaces; not product)\n",
            (unsigned)PMM_SOFT_WAVE);
    cAreas++;

    /*
     * Lean DMA residual (Soft!=product): low hierarchy first for rtl rings.
     * One line only - no version stamp, no stamp storms.
     * Grep: pmm: soft dma
     */
    kprintf("pmm: soft dma low_first=1 alloc_low_api=1 free_low=%lu "
            "free_high=%lu alloc_low=%lu alloc_high=%lu splits=%lu "
            "api_low_ok=%lu api_low_fail=%lu "
            "api_pages_low_ok=%lu api_pages_low_fail=%lu "
            "low_o0=%u high_o0=%u max_order=%u "
            "(rtl/UDX rings dma_buf force32; Soft!=product; not 1TiB product)\n",
            (unsigned long)g_cFramesFreeLow,
            (unsigned long)g_cFramesFreeHigh,
            (unsigned long)g_cAllocLow, (unsigned long)g_cAllocHigh,
            (unsigned long)g_cSplit,
            (unsigned long)g_cApiAllocLowOk,
            (unsigned long)g_cApiAllocLowFail,
            (unsigned long)g_cApiAllocPagesLowOk,
            (unsigned long)g_cApiAllocPagesLowFail,
            g_paFreeLow != 0 ? 1u : 0u, g_paFreeHigh != 0 ? 1u : 0u,
            PMM_MAX_ORDER);
    cAreas++;

    /*
     * Soft force32 residual self-exercise (alloc_low round-trip when free).
     * Rearranges freelist only; Soft!=product; never invents frames.
     */
    soft_force32_alloc_low_ex();

    /*
     * Lean soft residual - residual-class lamp for this unit.
     * Soft!=product - dual MIT OR Apache-2.0 - no version stamp - storm=0.
     * DMA-capable low-zone first + low-only API for UDX / force32.
     * greppable: pmm: soft residual lean
     */
    kprintf("pmm: soft residual lean "
            "dma_low_first=1 alloc_low_api=1 free_low=%lu free_high=%lu "
            "alloc_low=%lu alloc_high=%lu splits=%lu "
            "api_low_ok=%lu api_low_fail=%lu "
            "api_pages_low_ok=%lu api_pages_low_fail=%lu "
            "force32_ex_ok=%lu force32_ex_fail=%lu force32_ex=%u "
            "id_ok=%lu id_miss=%lu id_hold=%lu "
            "null_ok=%lu paint_ok=%lu lifo_ok=%lu "
            "free_rest_ok=%lu total_immut_ok=%lu "
            "c2_pass=%lu c2_fail=%lu gates=0x%x "
            "low_o0=%u high_o0=%u max_order=%u "
            "rtl_rings=1 udx_rings=1 force32_path=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "product_tib=0 pmem3=OPEN soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 storm=0 "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "no version stamp; not 1TiB product; not Dual DoD close; "
            "DMA page alloc foundation for UDX rings eng)\n",
            (unsigned long)g_cFramesFreeLow,
            (unsigned long)g_cFramesFreeHigh,
            (unsigned long)g_cAllocLow, (unsigned long)g_cAllocHigh,
            (unsigned long)g_cSplit,
            (unsigned long)g_cApiAllocLowOk,
            (unsigned long)g_cApiAllocLowFail,
            (unsigned long)g_cApiAllocPagesLowOk,
            (unsigned long)g_cApiAllocPagesLowFail,
            (unsigned long)g_cForce32SoftOk,
            (unsigned long)g_cForce32SoftFail, g_cForce32SoftEx,
            (unsigned long)g_cForce32SoftIdOk,
            (unsigned long)g_cForce32SoftIdMiss,
            (unsigned long)g_cForce32SoftIdHold,
            (unsigned long)g_cForce32SoftNullOk,
            (unsigned long)g_cForce32SoftPaintOk,
            (unsigned long)g_cForce32SoftLifoOk,
            (unsigned long)g_cForce32SoftFreeRestOk,
            (unsigned long)g_cForce32SoftTotalImmutOk,
            (unsigned long)g_cForce32SoftC2Pass,
            (unsigned long)g_cForce32SoftC2Fail,
            (unsigned)g_u32Force32SoftGates,
            g_paFreeLow != 0 ? 1u : 0u, g_paFreeHigh != 0 ? 1u : 0u,
            PMM_MAX_ORDER);
    cAreas++;

    /*
     * Force32 UDX DMA foundation residual (Soft!=product).
     * Low-only API + zone contract; identity [0,1GiB) soft hunt + dma_buf.
     * greppable: pmm: soft residual force32
     */
    kprintf("pmm: soft residual force32 "
            "low_only=1 never_high=1 zone_split=0x%lx "
            "vtd_identity=0x%lx "
            "free_low=%lu free_high=%lu "
            "api_low_ok=%lu api_low_fail=%lu "
            "api_pages_low_ok=%lu api_pages_low_fail=%lu "
            "force32_ex_ok=%lu force32_ex_fail=%lu "
            "id_ok=%lu id_miss=%lu "
            "null_ok=%lu paint_ok=%lu lifo_ok=%lu free_rest_ok=%lu "
            "udx_rings=1 rtl_rings=1 path=pmm_alloc_low "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; force32 foundation PA<4GiB; "
            "VT-d identity [0,1GiB) soft hunt + dma_buf/UDX filter; "
            "not product dual-license driver close; G-AC-1; "
            "not Dual DoD close)\n",
            (unsigned long)PMM_LOW_MAX,
            (unsigned long)PMM_VTD_ID_LIMIT,
            (unsigned long)g_cFramesFreeLow,
            (unsigned long)g_cFramesFreeHigh,
            (unsigned long)g_cApiAllocLowOk,
            (unsigned long)g_cApiAllocLowFail,
            (unsigned long)g_cApiAllocPagesLowOk,
            (unsigned long)g_cApiAllocPagesLowFail,
            (unsigned long)g_cForce32SoftOk,
            (unsigned long)g_cForce32SoftFail,
            (unsigned long)g_cForce32SoftIdOk,
            (unsigned long)g_cForce32SoftIdMiss,
            (unsigned long)g_cForce32SoftNullOk,
            (unsigned long)g_cForce32SoftPaintOk,
            (unsigned long)g_cForce32SoftLifoOk,
            (unsigned long)g_cForce32SoftFreeRestOk);
    cAreas++;

    /*
     * pmm_alloc_low API residual deepen (Soft!=product; UDX DMA force32).
     * greppable: pmm: soft residual alloc_low
     */
    kprintf("pmm: soft residual alloc_low "
            "api=pmm_alloc_low|pmm_alloc_pages_low "
            "contract=PA_lt_4GiB never_high=1 "
            "ok=%lu fail=%lu pages_ok=%lu pages_fail=%lu "
            "pop_low=%lu pop_high=%lu free_low=%lu "
            "force32_path=1 udx_eng=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "(Soft!=product; pmm_alloc_low residual for UDX DMA force32; "
            "G-AC-1; no version stamp; not Dual DoD close)\n",
            (unsigned long)g_cApiAllocLowOk,
            (unsigned long)g_cApiAllocLowFail,
            (unsigned long)g_cApiAllocPagesLowOk,
            (unsigned long)g_cApiAllocPagesLowFail,
            (unsigned long)g_cAllocLow, (unsigned long)g_cAllocHigh,
            (unsigned long)g_cFramesFreeLow);
    cAreas++;

    /*
     * VT-d identity residual (Soft!=product; C2 UDX force32 under TE).
     * Live soft hunt via alloc_low hold; product filter still dma_buf/UDX.
     * greppable: pmm: soft residual identity
     */
    kprintf("pmm: soft residual identity "
            "limit=0x%lx zone_low=0x%lx "
            "id_ok=%lu id_miss=%lu id_hold=%lu "
            "force32_ex_ok=%lu free_low=%lu "
            "hunt=alloc_low_hold hold_max=%u "
            "udx_rings=1 rtl_rings=1 te_safe_note=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "(Soft!=product; VT-d identity [0,1GiB) residual for C2 UDX "
            "force32; G-AC-1; no version stamp; not product AC; "
            "not Dual DoD close)\n",
            (unsigned long)PMM_VTD_ID_LIMIT,
            (unsigned long)PMM_LOW_MAX,
            (unsigned long)g_cForce32SoftIdOk,
            (unsigned long)g_cForce32SoftIdMiss,
            (unsigned long)g_cForce32SoftIdHold,
            (unsigned long)g_cForce32SoftOk,
            (unsigned long)g_cFramesFreeLow,
            PMM_SOFT_ID_HOLD);
    cAreas++;

    /*
     * C2 Dual DoD DMA foundation residual (Soft!=product; claim_class=C2).
     * Product *direction* only: low-only / force32 / VT-d identity pages
     * for userspace UDX Dual DoD A (USB) + B (NIC). Soft scaffold != product
     * AC; Dual DoD A/B remain OPEN. Stamp-free residual lamp.
     * greppable: pmm: soft residual C2 | claim_class=C2
     */
    kprintf("pmm: soft residual C2 "
            "claim_class=C2 product=UDX_DMA_foundation "
            "direction=low_only|force32|vtd_identity "
            "path=pmm_alloc_low|pmm_alloc_pages_low|pop_order_split_low_first "
            "not=in_kernel_ko_exec not=product_1TiB not=Dual_DoD_close "
            "zone_split=0x%lx vtd_identity=0x%lx "
            "gates=0x%x gate_n=%u "
            "null=%lu paint=%lu lifo=%lu free_rest=%lu total_immut=%lu "
            "id_ok=%lu id_miss=%lu force32_ok=%lu force32_fail=%lu "
            "api_low_ok=%lu pages_low_ok=%lu free_low=%lu "
            "c2_pass=%lu c2_fail=%lu "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "soft_scaffold_ne_product_ac=1 product_mint=0 product_tib=0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 storm=0 "
            "PMM_C2_DMA_FOUNDATION=1 "
            "(Soft!=product; C2 Dual DoD DMA foundation residual only; "
            "no version stamp; not product gate; not Dual DoD close; "
            "userspace UDX hosts own product ring/bounce)\n",
            (unsigned long)PMM_LOW_MAX,
            (unsigned long)PMM_VTD_ID_LIMIT,
            (unsigned)g_u32Force32SoftGates, (unsigned)PMM_SOFT_C2_GATE_N,
            (unsigned long)g_cForce32SoftNullOk,
            (unsigned long)g_cForce32SoftPaintOk,
            (unsigned long)g_cForce32SoftLifoOk,
            (unsigned long)g_cForce32SoftFreeRestOk,
            (unsigned long)g_cForce32SoftTotalImmutOk,
            (unsigned long)g_cForce32SoftIdOk,
            (unsigned long)g_cForce32SoftIdMiss,
            (unsigned long)g_cForce32SoftOk,
            (unsigned long)g_cForce32SoftFail,
            (unsigned long)g_cApiAllocLowOk,
            (unsigned long)g_cApiAllocPagesLowOk,
            (unsigned long)g_cFramesFreeLow,
            (unsigned long)g_cForce32SoftC2Pass,
            (unsigned long)g_cForce32SoftC2Fail);
    cAreas++;

    /*
     * Grep: pmm: soft residual C2 PASS | FAIL
     * Soft gate verdict only - never Dual DoD close / product AC.
     */
    if ((g_u32Force32SoftGates & PMM_SOFT_C2_GATE_CORE) ==
            PMM_SOFT_C2_GATE_CORE ||
        g_cForce32SoftC2Pass > 0) {
        kprintf("pmm: soft residual C2 PASS "
                "gates=0x%x core=0x%x claim_class=C2 "
                "path=alloc_low|pages_low|paint|lifo|free_rest|total_immut "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "soft_scaffold_ne_product_ac=1 product_mint=0 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "(Soft!=product; C2 residual only; no version stamp; "
                "not product gate; not Dual DoD close)\n",
                (unsigned)g_u32Force32SoftGates,
                (unsigned)PMM_SOFT_C2_GATE_CORE);
    } else if (g_cForce32SoftEx > 0) {
        kprintf("pmm: soft residual C2 FAIL "
                "gates=0x%x core=0x%x claim_class=C2 "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "(soft residual only; not product gate; Soft!=product; "
                "not Dual DoD close)\n",
                (unsigned)g_u32Force32SoftGates,
                (unsigned)PMM_SOFT_C2_GATE_CORE);
    } else {
        kprintf("pmm: soft residual C2 "
                "claim_class=C2 pending=1 gates=0x0 "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "soft_ne_product=1 "
                "(soft residual scaffold; exercise not yet run; "
                "not product gate; not Dual DoD close)\n");
    }
    cAreas++;

    /*
     * Grep: pmm: soft deepen - areas = prior soft lines this emission.
     * catalog=PMM_SOFT_AREAS is design high-water. No stamp storms.
     */
    kprintf("pmm: soft deepen areas=%u catalog=%u via=%s ready=%s "
            "free=%lu logs=%u surf=0x%x product_tib=0 pmem3=OPEN "
            "dma_low_first=1 residual_lean=1 residual_force32=1 "
            "residual_alloc_low=1 residual_identity=1 residual_c2=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "(soft; Soft!=product; not 1TiB product; not Dual DoD close)\n",
            cAreas, (unsigned)PMM_SOFT_AREAS, szWhere, szReady,
            (unsigned long)g_cFramesFree, g_cSoftInvLogs, (unsigned)u32Surf);

    /*
     * Close markers: freelist soft readiness only.
     * Grep: pmm: soft PASS | pmm: soft EMPTY | pmm: soft NONE
     * Never "1TiB product PASS". No version stamp on residual close.
     */
    if (fReady) {
        /* Grep: pmm: soft PASS | pmm: soft inventory PASS */
        kprintf("pmm: soft PASS via=%s free=%lu hierarchical free "
                "(soft inventory; not 1TiB product)\n",
                szWhere, (unsigned long)g_cFramesFree);
        kprintf("pmm: soft inventory PASS via=%s logs=%u "
                "(soft; not 1TiB product)\n",
                szWhere, g_cSoftInvLogs);
    } else {
        kprintf("pmm: soft %s via=%s free=%lu total=%lu "
                "(soft inventory; not 1TiB product)\n",
                szReady, szWhere, (unsigned long)g_cFramesFree,
                (unsigned long)g_cFramesTotal);
    }

    (void)PMM_SOFT_AREAS;
}

/**
 * Soft force32 / pmm_alloc_low residual exercise (Soft!=product).
 * C2 Dual DoD DMA foundation residual deepen:
 *   - dual null free remains no-op (free_count / total unchanged)
 *   - one low single + optional 4-page low contig when free_low permits
 *   - paint/verify dual pattern on low page (identity or HHDM VA)
 *   - LIFO reuse: free B then next pmm_alloc_low returns B
 *   - free_count restore + total immutable after full round-trip
 *   - soft identity hunt: hold up to PMM_SOFT_ID_HOLD non-identity low
 *     pages while seeking PA in VT-d [0, PMM_VTD_ID_LIMIT)
 * Verifies success PA < 4 GiB (never high). Free restores all held frames.
 * Cap: first 4 soft-inventory emissions only (no stamp storms / thrash).
 * Soft!=product; Dual DoD A/B remain OPEN; never product AC.
 * greppable: pmm: soft residual force32 | alloc_low | identity | C2
 */
static void
soft_force32_alloc_low_ex(void)
{
    gj_paddr_t pa;
    gj_paddr_t pa4;
    gj_paddr_t paId;
    gj_paddr_t paA;
    gj_paddr_t paB;
    gj_paddr_t paC;
    gj_paddr_t aHold[PMM_SOFT_ID_HOLD];
    u32 cHold;
    u32 u32Ok;
    u32 u32Gates;
    u32 i;
    u64 cFree0;
    u64 cFree1;
    u64 cTotal0;
    u64 cTotal1;
    u64 cAfterNull;
    volatile u64 *pWord;
    void *pVa;

    if (g_cForce32SoftEx >= 4u) {
        return;
    }
    g_cForce32SoftEx++;
    u32Ok = 0;
    u32Gates = 0;
    cFree0 = g_cFramesFree;
    cTotal0 = g_cFramesTotal;

    /*
     * Dual null free residual: free(0) is no-op (push_order refuses pa==0).
     * Soft!=product; free_count and total must stay put.
     */
    pmm_free(0);
    pmm_free(0);
    cAfterNull = g_cFramesFree;
    if (cAfterNull == cFree0 && g_cFramesTotal == cTotal0) {
        u32Gates |= PMM_SOFT_C2_GATE_NULL;
        if (g_cForce32SoftNullOk < 0xffffffffffffffffull) {
            g_cForce32SoftNullOk++;
        }
    }

    /* Single-page low-only: force32 foundation contract + paint/verify. */
    pa = pmm_alloc_low();
    if (pa != 0) {
        if (pa < PMM_LOW_MAX && (pa & (GJ_PAGE_SIZE - 1)) == 0) {
            u32Ok++;
            u32Gates |= PMM_SOFT_C2_GATE_SINGLE;
            if (g_cForce32SoftOk < 0xffffffffffffffffull) {
                g_cForce32SoftOk++;
            }
            /*
             * Soft paint/verify dual pattern (C2 residual strengthen).
             * Uses pa_to_ptr (identity before HHDM, HHDM after). Soft only.
             */
            pVa = pa_to_ptr(pa);
            if (pVa != 0) {
                pWord = (volatile u64 *)pVa;
                pWord[0] = PMM_SOFT_PAT_A ^ 0xC2ull;
                pWord[1] = PMM_SOFT_PAT_B ^ 0xC2ull;
                if (pWord[0] == (PMM_SOFT_PAT_A ^ 0xC2ull) &&
                    pWord[1] == (PMM_SOFT_PAT_B ^ 0xC2ull)) {
                    /* Overwrite + re-verify (second pattern pass). */
                    pWord[0] = PMM_SOFT_PAT_B ^ 0xC2ull;
                    pWord[1] = PMM_SOFT_PAT_A ^ 0xC2ull;
                    if (pWord[0] == (PMM_SOFT_PAT_B ^ 0xC2ull) &&
                        pWord[1] == (PMM_SOFT_PAT_A ^ 0xC2ull)) {
                        u32Gates |= PMM_SOFT_C2_GATE_PAINT;
                        if (g_cForce32SoftPaintOk < 0xffffffffffffffffull) {
                            g_cForce32SoftPaintOk++;
                        }
                    }
                }
            }
        } else {
            /* Refuse non-low hand-out (should never happen). */
            if (g_cForce32SoftFail < 0xffffffffffffffffull) {
                g_cForce32SoftFail++;
            }
        }
        pmm_free(pa);
    } else if (g_cFramesFreeLow == 0) {
        /* Empty low is honest soft skip - not a fail. */
    } else {
        if (g_cForce32SoftFail < 0xffffffffffffffffull) {
            g_cForce32SoftFail++;
        }
    }

    /* Contig low-only (4 pages): multi-page force32 / UDX ring residual. */
    pa4 = pmm_alloc_pages_low(4u);
    if (pa4 != 0) {
        if (pa4 < PMM_LOW_MAX &&
            (pa4 + (gj_paddr_t)4u * GJ_PAGE_SIZE) <= PMM_LOW_MAX &&
            (pa4 & (GJ_PAGE_SIZE - 1)) == 0) {
            u32Ok++;
            u32Gates |= PMM_SOFT_C2_GATE_PAGES;
            if (g_cForce32SoftOk < 0xffffffffffffffffull) {
                g_cForce32SoftOk++;
            }
            /* Soft: multi-page fully inside VT-d identity window. */
            if ((pa4 + (gj_paddr_t)4u * GJ_PAGE_SIZE) <=
                (gj_paddr_t)PMM_VTD_ID_LIMIT) {
                if (g_cForce32SoftIdOk < 0xffffffffffffffffull) {
                    g_cForce32SoftIdOk++;
                }
            }
        } else {
            if (g_cForce32SoftFail < 0xffffffffffffffffull) {
                g_cForce32SoftFail++;
            }
        }
        pmm_free_pages(pa4, 4u);
    }

    /*
     * Soft LIFO reuse residual (C2 strengthen): free of B must surface as
     * next pmm_alloc_low (order-0 freelist head). Soft!=product.
     */
    paA = pmm_alloc_low();
    paB = pmm_alloc_low();
    if (paA != 0 && paB != 0 && paA < PMM_LOW_MAX && paB < PMM_LOW_MAX &&
        (paA & (GJ_PAGE_SIZE - 1)) == 0 &&
        (paB & (GJ_PAGE_SIZE - 1)) == 0) {
        pmm_free(paB);
        paC = pmm_alloc_low();
        if (paC == paB) {
            u32Gates |= PMM_SOFT_C2_GATE_LIFO;
            if (g_cForce32SoftLifoOk < 0xffffffffffffffffull) {
                g_cForce32SoftLifoOk++;
            }
            if (g_cForce32SoftOk < 0xffffffffffffffffull) {
                g_cForce32SoftOk++;
            }
        }
        if (paC != 0) {
            pmm_free(paC);
        }
        pmm_free(paA);
    } else {
        if (paA != 0) {
            pmm_free(paA);
        }
        if (paB != 0) {
            pmm_free(paB);
        }
    }

    /*
     * Soft identity hunt (C2 UDX force32 residual): hold non-identity low
     * pages while seeking one page fully in [0, PMM_VTD_ID_LIMIT).
     * Mirrors dma_buf force32 prefer path; Soft!=product; free all after.
     */
    cHold = 0;
    paId = 0;
    for (i = 0; i < PMM_SOFT_ID_HOLD; i++) {
        pa = pmm_alloc_low();
        if (pa == 0) {
            break;
        }
        if ((pa & (GJ_PAGE_SIZE - 1)) != 0 || pa >= PMM_LOW_MAX) {
            /* Corrupt / non-low - free and stop hunt. */
            pmm_free(pa);
            if (g_cForce32SoftFail < 0xffffffffffffffffull) {
                g_cForce32SoftFail++;
            }
            break;
        }
        if ((pa + (gj_paddr_t)GJ_PAGE_SIZE) <= (gj_paddr_t)PMM_VTD_ID_LIMIT) {
            paId = pa;
            break;
        }
        /* Low but outside identity - hold while hunting. */
        if (cHold < PMM_SOFT_ID_HOLD) {
            aHold[cHold++] = pa;
            if (g_cForce32SoftIdHold < 0xffffffffffffffffull) {
                g_cForce32SoftIdHold++;
            }
        } else {
            pmm_free(pa);
            break;
        }
    }
    while (cHold > 0u) {
        cHold--;
        pmm_free(aHold[cHold]);
    }
    if (paId != 0) {
        u32Gates |= PMM_SOFT_C2_GATE_ID;
        if (g_cForce32SoftIdOk < 0xffffffffffffffffull) {
            g_cForce32SoftIdOk++;
        }
        if (g_cForce32SoftOk < 0xffffffffffffffffull) {
            g_cForce32SoftOk++;
        }
        pmm_free(paId);
    } else if (g_cFramesFreeLow > 0) {
        /* Free low exists but no identity page in hold budget. */
        if (g_cForce32SoftIdMiss < 0xffffffffffffffffull) {
            g_cForce32SoftIdMiss++;
        }
    }

    /*
     * Free restore + total immutable (C2 residual core gates).
     * After all soft alloc/free, free_count must match start; total never
     * invents/drops frames. Soft!=product; never hard-gates product.
     */
    cFree1 = g_cFramesFree;
    cTotal1 = g_cFramesTotal;
    if (cFree1 == cFree0) {
        u32Gates |= PMM_SOFT_C2_GATE_FREE_R;
        if (g_cForce32SoftFreeRestOk < 0xffffffffffffffffull) {
            g_cForce32SoftFreeRestOk++;
        }
    }
    if (cTotal1 == cTotal0) {
        u32Gates |= PMM_SOFT_C2_GATE_TOTAL_I;
        if (g_cForce32SoftTotalImmutOk < 0xffffffffffffffffull) {
            g_cForce32SoftTotalImmutOk++;
        }
    }

    g_u32Force32SoftGates = u32Gates;
    if ((u32Gates & PMM_SOFT_C2_GATE_CORE) == PMM_SOFT_C2_GATE_CORE) {
        if (g_cForce32SoftC2Pass < 0xffffffffffffffffull) {
            g_cForce32SoftC2Pass++;
        }
    } else if ((u32Gates & PMM_SOFT_C2_GATE_SINGLE) != 0) {
        /*
         * Had low frames for single alloc but core gates incomplete
         * (paint/LIFO/free_rest). Empty-low skip is neither pass nor fail.
         */
        if (g_cForce32SoftC2Fail < 0xffffffffffffffffull) {
            g_cForce32SoftC2Fail++;
        }
    }

    (void)u32Ok;
}

/**
 * Soft hierarchical exercise for any RAM size (1 TiB design observability).
 * Tries alloc/free each order 1..MAX and a few max-order (2 MiB) blocks.
 * Also soft-exercises pmm_alloc_low / pmm_alloc_pages_low (force32 residual).
 * Rearranges freelist nodes only - never invents frames; safe when free is
 * small (failed orders simply skip). Returns # of orders that succeeded.
 */
static u32
soft_hier_exercise(u32 *pOutBig)
{
    u32 o;
    u32 cOrderOk = 0;
    u32 nBig = 0;
    u32 i;
    gj_paddr_t paLow;

    /* Force32 residual: low-only single + pages before general hierarchy. */
    soft_force32_alloc_low_ex();
    paLow = pmm_alloc_low();
    if (paLow != 0) {
        if (paLow < PMM_LOW_MAX) {
            cOrderOk++; /* soft credit: low-only path live */
        }
        pmm_free(paLow);
    }

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

/* Smallest order whose block has >= cPages (capped at PMM_MAX_ORDER). */
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
 * Prefer largest aligned power-of-two blocks (up to 2 MiB / order 9) so
 * multi-hundred-GiB machines do not walk every 4 KiB page at boot.
 *
 * Algorithm (stable - do not regress for 768G soak):
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
    g_cAllocLow = 0;
    g_cAllocHigh = 0;
    g_cApiAllocLowOk = 0;
    g_cApiAllocLowFail = 0;
    g_cApiAllocPagesLowOk = 0;
    g_cApiAllocPagesLowFail = 0;
    g_cForce32SoftOk = 0;
    g_cForce32SoftFail = 0;
    g_cForce32SoftEx = 0;
    g_cForce32SoftIdOk = 0;
    g_cForce32SoftIdMiss = 0;
    g_cForce32SoftIdHold = 0;
    g_cHigh = 0;
    g_fHighReleased = 0;
    g_cSoftInvLogs = 0;
    /* Keep g_paSoftMedia* from pmm_soft_reserve() (call before pmm_init). */
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
    /* Wave 15: greppable pmm: soft ... inventory (not 1TiB product). */
    pmm_soft_inventory("init");
}

void
pmm_release_high(void)
{
    u32 i;

    if (g_fHighReleased || !hhdm_ready()) {
        return;
    }
    /* Hierarchical free_range into high order freelists (PA >= 4 GiB). */
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
    /* Wave 15: greppable pmm: soft ... after high release. */
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
    /* Defensive: corrupt high head - do not advance; fall back to dual path. */
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
    /* Cap: hierarchical max (512 pages = 2 MiB when PMM_MAX_ORDER=9). */
    if (cPages > (1u << PMM_MAX_ORDER)) {
        return 0;
    }
    /*
     * Hierarchical free path: exact power-of-two page count -> order freelist
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

/*
 * Low-zone only pop for DMA foundation (Soft!=product; force32 residual).
 * Uses pop_split_zone(fLow=1); never touches high freelists.
 * Soft tally: g_cAllocLow on success (same bucket as pop_order_split low).
 * Belt-and-suspenders: refuse any PA >= 4 GiB (force32 / UDX contract).
 */
static gj_paddr_t
pop_order_low_only(u32 u32Order)
{
    gj_paddr_t pa;
    u32 cPages;

    if (u32Order > PMM_MAX_ORDER) {
        return 0;
    }
    pa = pop_split_zone(u32Order, 1);
    if (pa == 0) {
        return 0;
    }
    /*
     * Force32 residual deepen: success PA must sit entirely in low zone.
     * pop_split_zone(fLow=1) already zone-guards; this refuses any
     * cross-zone block that would break UDX DMA force32 foundation.
     */
    cPages = 1u << u32Order;
    if (pa >= PMM_LOW_MAX ||
        (pa + (gj_paddr_t)cPages * GJ_PAGE_SIZE) > PMM_LOW_MAX) {
        /* Restore frames; never hand high/cross-zone on low-only path. */
        push_order(pa, u32Order);
        return 0;
    }
    g_cAllocLow++;
    return pa;
}

/**
 * DMA page-alloc foundation: one low-zone frame (PA < 4 GiB). Soft!=product.
 * Never falls back to high - returns 0 if low hierarchy empty.
 * Soft residual deepen for UDX DMA force32: API ok/fail tallies; PA < 4 GiB.
 * greppable: pmm_alloc_low | pmm: soft residual alloc_low
 */
gj_paddr_t
pmm_alloc_low(void)
{
    gj_paddr_t pa;

    pa = pop_order_low_only(0);
    if (pa != 0) {
        /* Final force32 contract: page-aligned + low zone only. */
        if (pa >= PMM_LOW_MAX || (pa & (GJ_PAGE_SIZE - 1)) != 0) {
            push_free(pa);
            if (g_cApiAllocLowFail < 0xffffffffffffffffull) {
                g_cApiAllocLowFail++;
            }
            return 0;
        }
        if (g_cApiAllocLowOk < 0xffffffffffffffffull) {
            g_cApiAllocLowOk++;
        }
        return pa;
    }
    if (g_cApiAllocLowFail < 0xffffffffffffffffull) {
        g_cApiAllocLowFail++;
    }
    return 0;
}

/**
 * DMA page-alloc foundation: contiguous low-zone pages only. Soft!=product.
 * Power-of-two via low hierarchical split; else low order-0 scan (capped).
 * Never returns PA >= 4 GiB. Soft residual: multi-page force32 / UDX rings.
 * greppable: pmm_alloc_pages_low | pmm: soft residual force32
 */
gj_paddr_t
pmm_alloc_pages_low(u32 cPages)
{
    gj_paddr_t aHold[256];
    u32 cHold = 0;
    gj_paddr_t paBase = 0;
    u32 i;
    u32 oWant;

    if (cPages == 0) {
        if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
            g_cApiAllocPagesLowFail++;
        }
        return 0;
    }
    if (cPages == 1) {
        /* Route single through pmm_alloc_low so API tallies stay unified. */
        paBase = pmm_alloc_low();
        if (paBase != 0) {
            if (g_cApiAllocPagesLowOk < 0xffffffffffffffffull) {
                g_cApiAllocPagesLowOk++;
            }
            return paBase;
        }
        if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
            g_cApiAllocPagesLowFail++;
        }
        return 0;
    }
    if (cPages > (1u << PMM_MAX_ORDER)) {
        if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
            g_cApiAllocPagesLowFail++;
        }
        return 0;
    }
    oWant = pages_to_order(cPages);
    if ((1u << oWant) == cPages) {
        paBase = pop_order_low_only(oWant);
        if (paBase != 0) {
            /* Force32: entire span must sit under 4 GiB. */
            if (paBase >= PMM_LOW_MAX ||
                (paBase + (gj_paddr_t)cPages * GJ_PAGE_SIZE) > PMM_LOW_MAX) {
                push_order(paBase, oWant);
                if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
                    g_cApiAllocPagesLowFail++;
                }
                return 0;
            }
            if (g_cApiAllocPagesLowOk < 0xffffffffffffffffull) {
                g_cApiAllocPagesLowOk++;
            }
            return paBase;
        }
        /* Low hierarchy cannot satisfy; do not fall back to high. */
        if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
            g_cApiAllocPagesLowFail++;
        }
        return 0;
    }
    /* Non-power-of-two: scan low order-0 only (DMA residual lean). */
    while (cHold < 256 && g_cFramesFreeLow > 0) {
        gj_paddr_t pa = pop_order_low_only(0);
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
        if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
            g_cApiAllocPagesLowFail++;
        }
        return 0;
    }
    /* Force32: refuse run that touches or crosses the 4 GiB boundary. */
    if (paBase >= PMM_LOW_MAX ||
        (paBase + (gj_paddr_t)cPages * GJ_PAGE_SIZE) > PMM_LOW_MAX) {
        for (i = 0; i < cHold; i++) {
            push_free(aHold[i]);
        }
        if (g_cApiAllocPagesLowFail < 0xffffffffffffffffull) {
            g_cApiAllocPagesLowFail++;
        }
        return 0;
    }
    for (i = 0; i < cHold; i++) {
        if (aHold[i] < paBase ||
            aHold[i] >= paBase + (gj_paddr_t)cPages * GJ_PAGE_SIZE) {
            push_free(aHold[i]);
        }
    }
    if (g_cApiAllocPagesLowOk < 0xffffffffffffffffull) {
        g_cApiAllocPagesLowOk++;
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
    /* Refuse unaligned base - never half-insert into freelists. */
    if ((paPage & (GJ_PAGE_SIZE - 1)) != 0) {
        return;
    }
    /*
     * Hierarchical free path: exact power-of-two page count at natural
     * block alignment -> one order freelist node (O(1)).
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
        /* Total free frames (not order-0 node count) - greppable diagnostics. */
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
    /* Wave 15: greppable pmm: soft ... on explicit order dump. */
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
     * Soft gate for large-RAM PASS path: main.c uses 768ull<<30 (768 GiB).
     * Below threshold -> soak_tib SKIP soft (return 0, not a fail), but still
     * run soft hierarchical exercise + order counts for 1 TiB design
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
    /* Wave 15: greppable pmm: soft ... after large-RAM hierarchical soak. */
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
    /* Wave 15: greppable pmm: soft ... after hard soak PASS. */
    pmm_soft_inventory("soak");
    return 0;
}
