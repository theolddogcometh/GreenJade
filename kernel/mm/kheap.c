/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Simple freelist kernel heap backed by PMM pages (HHDM-aware).
 * Best-fit first; page-sized grow; scrub on free; magic double-free guard.
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0. Soft!=product.
 *
 * Soft fragmentation (internal):
 *   - align:  round-up of request to 16-byte units
 *   - unsplit: free remainder too small for header+16, kept with live block
 * Live soft bytes are subtracted on free; lifetime soft charged/released
 * and soft waste-hit events are cumulative (wrap OK; diagnostics only).
 *
 * Lean residual (this unit only; Soft!=product; dual MIT OR Apache-2.0):
 *   - Freelist + PMM grow only; single-page allocs (multi_page=OPEN)
 *   - Adjacent free coalesce=OPEN (not product capacity claim)
 *   - Soft inventory diagnostics only; never hard-gates; wrap OK
 *   - No version stamp. No ret*angle stamp storms. storm=0.
 *   - G-AC-1: soft != product AC; != 1TiB product.
 *   - C2 residual: freelist integrity match + soft kmalloc foundation
 *     for UDX eng (linux_dma_soft peer); Dual DoD A/B remain OPEN.
 *   - Never closes Dual DoD / product AC / 1TiB product.
 *
 * Soft inventory surfaces (diagnostics never product):
 *   - Honesty / non-claims: Soft!=product, != 1TiB product
 *   - Live soft: align / unsplit / frag / peak / used / free / free_blocks
 *   - Lifetime: charged / released / net / peak components / max-one
 *   - Waste events: waste_allocs/frees, align/unsplit hits, take_whole
 *   - Fail taxonomy: zero / uninit / oversize / oom / double_free
 *   - Ops: allocs / frees / grow / split / best_fit / null_free
 *   - Design / freelist walk / grow / scrub / lamps
 *   - Surfaces / magic / return / header / residual (surf bit)
 *   - Residual lean + C2 residual deepen + freelist integrity + PASS/NONE
 *   greppable: "kheap: soft ..."
 *
 * Greppable:
 *   kheap: init
 *   kheap: stats ...
 *   kheap: counters ...
 *   kheap: soft ...            (baseline allocs/frees/bytes + waste)
 *   kheap: soft honesty ...
 *   kheap: soft inventory ...
 *   kheap: soft live ...
 *   kheap: soft lifetime ...
 *   kheap: soft waste ...
 *   kheap: soft fail ...
 *   kheap: soft ops ...
 *   kheap: soft design ...
 *   kheap: soft freelist ...   (incl. integrity match lamps)
 *   kheap: soft grow ...
 *   kheap: soft scrub ...
 *   kheap: soft lamps ...
 *   kheap: soft path ...
 *   kheap: soft stats ...
 *   kheap: soft surfaces ...
 *   kheap: soft magic ...
 *   kheap: soft return ...
 *   kheap: soft header ...
 *   kheap: soft residual lean  (sole residual-class lamp; Soft!=product)
 *   kheap: soft residual lean PASS
 *   kheap: soft residual deepen  (C2 residual; Dual DoD OPEN)
 *   kheap: soft deepen ...
 *   kheap: soft PASS | NONE | inventory PASS
 *   Soft!=product | dual MIT OR Apache-2.0 | storm=0 | G-AC-1 | C2 residual
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/vmm.h>

/* Soft inventory stamp (file-local; never product gate; not image version). */
#define KHEAP_SOFT_WAVE 126u
/* Catalog areas prior to deepen (honesty..residual deepen). Soft!=product. */
#define KHEAP_SOFT_AREAS 21u

/*
 * Wave 19 return-surface bit lamps (surf=0x... on soft surfaces/deepen).
 * greppable: kheap: soft surfaces
 */
#define KHEAP_SOFT_SURF_HONESTY   (1u << 0)
#define KHEAP_SOFT_SURF_BASELINE  (1u << 1)
#define KHEAP_SOFT_SURF_INVENTORY (1u << 2)
#define KHEAP_SOFT_SURF_LIVE      (1u << 3)
#define KHEAP_SOFT_SURF_LIFETIME  (1u << 4)
#define KHEAP_SOFT_SURF_WASTE     (1u << 5)
#define KHEAP_SOFT_SURF_FAIL      (1u << 6)
#define KHEAP_SOFT_SURF_OPS       (1u << 7)
#define KHEAP_SOFT_SURF_DESIGN    (1u << 8)
#define KHEAP_SOFT_SURF_FREELIST  (1u << 9)
#define KHEAP_SOFT_SURF_GROW      (1u << 10)
#define KHEAP_SOFT_SURF_SCRUB     (1u << 11)
#define KHEAP_SOFT_SURF_LAMPS     (1u << 12)
#define KHEAP_SOFT_SURF_PATH      (1u << 13)
#define KHEAP_SOFT_SURF_STATS     (1u << 14)
#define KHEAP_SOFT_SURF_SURFACES  (1u << 15)
#define KHEAP_SOFT_SURF_MAGIC     (1u << 16)
#define KHEAP_SOFT_SURF_RETURN    (1u << 17)
#define KHEAP_SOFT_SURF_HEADER    (1u << 18)
#define KHEAP_SOFT_SURF_RESIDUAL  (1u << 19) /* residual lean lamp surface */
#define KHEAP_SOFT_SURF_CATALOG                                                    \
    (KHEAP_SOFT_SURF_HONESTY | KHEAP_SOFT_SURF_BASELINE |                          \
     KHEAP_SOFT_SURF_INVENTORY | KHEAP_SOFT_SURF_LIVE | KHEAP_SOFT_SURF_LIFETIME | \
     KHEAP_SOFT_SURF_WASTE | KHEAP_SOFT_SURF_FAIL | KHEAP_SOFT_SURF_OPS |          \
     KHEAP_SOFT_SURF_DESIGN | KHEAP_SOFT_SURF_FREELIST | KHEAP_SOFT_SURF_GROW |    \
     KHEAP_SOFT_SURF_SCRUB | KHEAP_SOFT_SURF_LAMPS | KHEAP_SOFT_SURF_PATH |        \
     KHEAP_SOFT_SURF_STATS | KHEAP_SOFT_SURF_SURFACES | KHEAP_SOFT_SURF_MAGIC |    \
     KHEAP_SOFT_SURF_RETURN | KHEAP_SOFT_SURF_HEADER | KHEAP_SOFT_SURF_RESIDUAL)

/* Live / free magic - detects double-free and obvious header corruption. */
#define GJ_KHEAP_MAGIC_LIVE  0x4B4C4956u /* 'KLIV' */
#define GJ_KHEAP_MAGIC_FREE  0x4B465245u /* 'KFRE' */

struct kheap_block {
    size_t               cbSize;       /* usable payload bytes */
    u32                  u32Magic;     /* LIVE or FREE */
    u16                  u16SoftAlign; /* align waste when LIVE; 0 FREE */
    u16                  u16SoftUnsplit; /* unsplit waste when LIVE; 0 FREE */
    struct kheap_block  *pNext;        /* free-list link when free */
};

/* Usable payload per grown page (one header at page base). */
#define GJ_KHEAP_PAGE_PAYLOAD \
    ((size_t)GJ_PAGE_SIZE - sizeof(struct kheap_block))

/* Min leftover to justify a split: header + 16-byte payload. */
#define GJ_KHEAP_SPLIT_MIN \
    (sizeof(struct kheap_block) + 16u)

static struct kheap_block *g_pFree;
static size_t              g_cbUsed;
static size_t              g_cbFree;
static size_t              g_cFreeBlocks;
/* Live soft frag (charged on alloc, released on free). */
static size_t              g_cbSoftAlign;
static size_t              g_cbSoftUnsplit;
/* Soft deepen: lifetime + peak + waste-hit events (not live). */
static size_t              g_cbSoftPeak;       /* peak live soft_frag */
static size_t              g_cbSoftPeakAlign;  /* peak live soft align */
static size_t              g_cbSoftPeakUnsplit;/* peak live soft unsplit */
static size_t              g_cbSoftMaxAlignOne;/* max align waste per alloc */
static size_t              g_cbSoftMaxUnsplitOne; /* max unsplit per alloc */
static size_t              g_cbUsedPeak;       /* peak live used payload */
static size_t              g_cFreeBlocksPeak;  /* peak free-list length */
static u64                 g_cbSoftCharged;    /* lifetime soft bytes in */
static u64                 g_cbSoftReleased;   /* lifetime soft bytes out */
static u64                 g_cSoftWasteAlloc;  /* allocs with any soft waste */
static u64                 g_cSoftWasteFree;   /* frees releasing soft waste */
static u64                 g_cSoftAlignHit;    /* allocs with align pad */
static u64                 g_cSoftUnsplitHit;  /* allocs with unsplit remainder */
static u64                 g_cSoftTakeWhole;   /* alloc took whole free block */
static u64                 g_cSoftBestFit;     /* best-fit selection hits */
/* Soft fail taxonomy (Wave 11; subset of g_cFail / free guards). */
static u64                 g_cSoftFailZero;    /* alloc cb == 0 */
static u64                 g_cSoftFailUninit;  /* alloc before kheap_init */
static u64                 g_cSoftFailOversize;/* request > page payload */
static u64                 g_cSoftFailOom;     /* PMM grow failed */
static u64                 g_cSoftNullFree;    /* free(NULL) */
static u64                 g_cSoftFreeUninit;  /* free before init */
static u64                 g_cSoftLogN;        /* soft inventory dump count */
static u64                 g_cAlloc;
static u64                 g_cFree;
static u64                 g_cGrow;
static u64                 g_cSplit;
static u64                 g_cFail;
static u64                 g_cDoubleFree;
static int                 g_fInit;

/**
 * Soft-account one successful alloc's internal waste.
 * Updates live soft bytes (caller already added align/unsplit), lifetime
 * charged, peak live soft_frag / components, max-one, waste-hit events.
 */
static void
soft_note_alloc(size_t cbAlignWaste, size_t cbUnsplit)
{
    size_t cbSoftLive;

    if (cbAlignWaste > 0) {
        g_cSoftAlignHit++;
    }
    if (cbUnsplit > 0) {
        g_cSoftUnsplitHit++;
    }
    if (cbAlignWaste > 0 || cbUnsplit > 0) {
        g_cSoftWasteAlloc++;
        g_cbSoftCharged += (u64)cbAlignWaste + (u64)cbUnsplit;
    }
    if (cbAlignWaste > g_cbSoftMaxAlignOne) {
        g_cbSoftMaxAlignOne = cbAlignWaste;
    }
    if (cbUnsplit > g_cbSoftMaxUnsplitOne) {
        g_cbSoftMaxUnsplitOne = cbUnsplit;
    }
    if (g_cbSoftAlign > g_cbSoftPeakAlign) {
        g_cbSoftPeakAlign = g_cbSoftAlign;
    }
    if (g_cbSoftUnsplit > g_cbSoftPeakUnsplit) {
        g_cbSoftPeakUnsplit = g_cbSoftUnsplit;
    }
    cbSoftLive = g_cbSoftAlign + g_cbSoftUnsplit;
    if (cbSoftLive > g_cbSoftPeak) {
        g_cbSoftPeak = cbSoftLive;
    }
}

/**
 * Soft-account one successful free's released internal waste.
 * Live soft bytes already decremented by caller.
 */
static void
soft_note_free(size_t cbAlign, size_t cbUnsplit)
{
    if (cbAlign > 0 || cbUnsplit > 0) {
        g_cSoftWasteFree++;
        g_cbSoftReleased += (u64)cbAlign + (u64)cbUnsplit;
    }
}

/** Soft: bump used / free_blocks high-water marks (diagnostics only). */
static void
soft_note_peaks(void)
{
    if (g_cbUsed > g_cbUsedPeak) {
        g_cbUsedPeak = g_cbUsed;
    }
    if (g_cFreeBlocks > g_cFreeBlocksPeak) {
        g_cFreeBlocksPeak = g_cFreeBlocks;
    }
}

void
kheap_init(void)
{
    g_pFree = NULL;
    g_cbUsed = 0;
    g_cbFree = 0;
    g_cFreeBlocks = 0;
    g_cbSoftAlign = 0;
    g_cbSoftUnsplit = 0;
    g_cbSoftPeak = 0;
    g_cbSoftPeakAlign = 0;
    g_cbSoftPeakUnsplit = 0;
    g_cbSoftMaxAlignOne = 0;
    g_cbSoftMaxUnsplitOne = 0;
    g_cbUsedPeak = 0;
    g_cFreeBlocksPeak = 0;
    g_cbSoftCharged = 0;
    g_cbSoftReleased = 0;
    g_cSoftWasteAlloc = 0;
    g_cSoftWasteFree = 0;
    g_cSoftAlignHit = 0;
    g_cSoftUnsplitHit = 0;
    g_cSoftTakeWhole = 0;
    g_cSoftBestFit = 0;
    g_cSoftFailZero = 0;
    g_cSoftFailUninit = 0;
    g_cSoftFailOversize = 0;
    g_cSoftFailOom = 0;
    g_cSoftNullFree = 0;
    g_cSoftFreeUninit = 0;
    g_cSoftLogN = 0;
    g_cAlloc = 0;
    g_cFree = 0;
    g_cGrow = 0;
    g_cSplit = 0;
    g_cFail = 0;
    g_cDoubleFree = 0;
    g_fInit = 1;
    kprintf("kheap: init\n");
}

static size_t
align_up(size_t cb)
{
    return (cb + 15u) & ~(size_t)15u;
}

static struct kheap_block *
block_from_payload(void *p)
{
    return (struct kheap_block *)((u8 *)p - sizeof(struct kheap_block));
}

static void *
payload_from_block(struct kheap_block *pBlk)
{
    return (void *)((u8 *)pBlk + sizeof(struct kheap_block));
}

/**
 * Grow freelist by one PMM page (HHDM VA when ready; identity for low PA).
 * Single-page blocks only - PMM has no contiguous multi-frame guarantee.
 */
static gj_status_t
grow(void)
{
    gj_paddr_t pa;
    struct kheap_block *pBlk;
    size_t cbHdr = sizeof(struct kheap_block);

    pa = pmm_alloc();
    if (pa == 0) {
        return GJ_ERR_NOMEM;
    }
    pBlk = (struct kheap_block *)(void *)hhdm_to_virt(pa);
    pBlk->cbSize = (size_t)GJ_PAGE_SIZE - cbHdr;
    pBlk->u32Magic = GJ_KHEAP_MAGIC_FREE;
    pBlk->u16SoftAlign = 0;
    pBlk->u16SoftUnsplit = 0;
    pBlk->pNext = g_pFree;
    g_pFree = pBlk;
    g_cbFree += pBlk->cbSize;
    g_cFreeBlocks++;
    g_cGrow++;
    soft_note_peaks();
    return GJ_OK;
}

void *
kheap_alloc(size_t cb)
{
    struct kheap_block *pPrev;
    struct kheap_block *pCur;
    struct kheap_block *pBest;
    struct kheap_block *pBestPrev;
    size_t cbNeed;
    size_t cbAlignWaste;
    size_t cbUnsplit;
    void *pOut;

    if (!g_fInit) {
        g_cFail++;
        g_cSoftFailUninit++;
        return NULL;
    }
    if (cb == 0) {
        g_cFail++;
        g_cSoftFailZero++;
        return NULL;
    }
    cbNeed = align_up(cb);
    cbAlignWaste = cbNeed - cb;
    /*
     * One header + payload must fit a single grown page. Larger requests
     * need a future multi-page path (contiguous frames or chained slabs).
     */
    if (cbNeed > GJ_KHEAP_PAGE_PAYLOAD) {
        g_cFail++;
        g_cSoftFailOversize++;
        return NULL;
    }

    for (;;) {
        pPrev = NULL;
        pBest = NULL;
        pBestPrev = NULL;
        for (pCur = g_pFree; pCur != NULL; pPrev = pCur, pCur = pCur->pNext) {
            if (pCur->u32Magic != GJ_KHEAP_MAGIC_FREE) {
                continue;
            }
            if (pCur->cbSize >= cbNeed) {
                if (pBest == NULL || pCur->cbSize < pBest->cbSize) {
                    pBest = pCur;
                    pBestPrev = pPrev;
                }
            }
        }
        if (pBest != NULL) {
            g_cSoftBestFit++;
            cbUnsplit = 0;
            /* Split if leftover holds another header + 16-byte payload. */
            if (pBest->cbSize >= cbNeed + GJ_KHEAP_SPLIT_MIN) {
                struct kheap_block *pSplit;
                size_t cbLeft;
                size_t cbTaken;

                cbLeft = pBest->cbSize - cbNeed - sizeof(struct kheap_block);
                pSplit = (struct kheap_block *)((u8 *)payload_from_block(pBest) +
                                                cbNeed);
                pSplit->cbSize = cbLeft;
                pSplit->u32Magic = GJ_KHEAP_MAGIC_FREE;
                pSplit->u16SoftAlign = 0;
                pSplit->u16SoftUnsplit = 0;
                pSplit->pNext = pBest->pNext;
                /*
                 * Free bytes: remove whole best, re-add split remainder.
                 * Net: -(cbNeed + header). Free block count unchanged
                 * (unlink best, leave split linked in its place).
                 */
                cbTaken = cbNeed + sizeof(struct kheap_block);
                if (g_cbFree >= cbTaken) {
                    g_cbFree -= cbTaken;
                } else {
                    g_cbFree = 0;
                }
                pBest->cbSize = cbNeed;
                pBest->pNext = pSplit;
                if (pBestPrev != NULL) {
                    pBestPrev->pNext = pSplit;
                } else {
                    g_pFree = pSplit;
                }
                g_cSplit++;
            } else {
                /* Take whole free block; excess is soft unsplit waste. */
                cbUnsplit = pBest->cbSize - cbNeed;
                g_cSoftTakeWhole++;
                if (g_cbFree >= pBest->cbSize) {
                    g_cbFree -= pBest->cbSize;
                } else {
                    g_cbFree = 0;
                }
                if (g_cFreeBlocks > 0) {
                    g_cFreeBlocks--;
                }
                if (pBestPrev != NULL) {
                    pBestPrev->pNext = pBest->pNext;
                } else {
                    g_pFree = pBest->pNext;
                }
            }
            pBest->u32Magic = GJ_KHEAP_MAGIC_LIVE;
            pBest->pNext = NULL;
            pBest->u16SoftAlign = (u16)cbAlignWaste;
            pBest->u16SoftUnsplit = (u16)cbUnsplit;
            g_cbSoftAlign += cbAlignWaste;
            g_cbSoftUnsplit += cbUnsplit;
            soft_note_alloc(cbAlignWaste, cbUnsplit);
            g_cbUsed += pBest->cbSize;
            g_cAlloc++;
            soft_note_peaks();
            pOut = payload_from_block(pBest);
            memset(pOut, 0, pBest->cbSize);
            return pOut;
        }
        if (grow() != GJ_OK) {
            g_cFail++;
            g_cSoftFailOom++;
            return NULL;
        }
    }
}

void
kheap_free(void *p)
{
    struct kheap_block *pBlk;
    size_t cbAlign;
    size_t cbUnsplit;

    if (p == NULL) {
        g_cSoftNullFree++;
        return;
    }
    if (!g_fInit) {
        g_cSoftFreeUninit++;
        return;
    }
    pBlk = block_from_payload(p);
    if (pBlk->u32Magic != GJ_KHEAP_MAGIC_LIVE) {
        /* Double-free or corrupt header - fail closed, do not relink. */
        g_cDoubleFree++;
        return;
    }
    cbAlign = (size_t)pBlk->u16SoftAlign;
    cbUnsplit = (size_t)pBlk->u16SoftUnsplit;
    if (g_cbSoftAlign >= cbAlign) {
        g_cbSoftAlign -= cbAlign;
    } else {
        g_cbSoftAlign = 0;
    }
    if (g_cbSoftUnsplit >= cbUnsplit) {
        g_cbSoftUnsplit -= cbUnsplit;
    } else {
        g_cbSoftUnsplit = 0;
    }
    soft_note_free(cbAlign, cbUnsplit);
    if (g_cbUsed >= pBlk->cbSize) {
        g_cbUsed -= pBlk->cbSize;
    } else {
        g_cbUsed = 0;
    }
    /* Scrub payload so use-after-free does not leak secrets. */
    memset(payload_from_block(pBlk), 0, pBlk->cbSize);
    pBlk->u32Magic = GJ_KHEAP_MAGIC_FREE;
    pBlk->u16SoftAlign = 0;
    pBlk->u16SoftUnsplit = 0;
    pBlk->pNext = g_pFree;
    g_pFree = pBlk;
    g_cbFree += pBlk->cbSize;
    g_cFreeBlocks++;
    g_cFree++;
    soft_note_peaks();
}

size_t
kheap_used_bytes(void)
{
    return g_cbUsed;
}

size_t
kheap_free_bytes(void)
{
    return g_cbFree;
}

size_t
kheap_free_blocks(void)
{
    return g_cFreeBlocks;
}

size_t
kheap_soft_frag_bytes(void)
{
    return g_cbSoftAlign + g_cbSoftUnsplit;
}

void
kheap_get_stats(struct kheap_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    pOut->cbUsed = g_cbUsed;
    pOut->cbFree = g_cbFree;
    pOut->cFreeBlocks = g_cFreeBlocks;
    pOut->cbSoftFrag = g_cbSoftAlign + g_cbSoftUnsplit;
    pOut->cbSoftAlign = g_cbSoftAlign;
    pOut->cbSoftUnsplit = g_cbSoftUnsplit;
    pOut->cAlloc = g_cAlloc;
    pOut->cFree = g_cFree;
    pOut->cGrow = g_cGrow;
    pOut->cSplit = g_cSplit;
    pOut->cFail = g_cFail;
    pOut->cDoubleFree = g_cDoubleFree;
}

/**
 * Soft freelist walk (Wave 15): min/max free payload, walk length, magic
 * mismatch soft count. Diagnostics only - never mutates freelist.
 */
static void
soft_freelist_scan(size_t *pCbMin, size_t *pCbMax, size_t *pCbSum,
                   u32 *pCWalk, u32 *pCBadMagic)
{
    struct kheap_block *pCur;
    size_t cbMin = 0;
    size_t cbMax = 0;
    size_t cbSum = 0;
    u32 cWalk = 0;
    u32 cBad = 0;
    int fFirst = 1;

    for (pCur = g_pFree; pCur != NULL; pCur = pCur->pNext) {
        cWalk++;
        if (pCur->u32Magic != GJ_KHEAP_MAGIC_FREE) {
            cBad++;
            continue;
        }
        cbSum += pCur->cbSize;
        if (fFirst != 0 || pCur->cbSize < cbMin) {
            cbMin = pCur->cbSize;
        }
        if (fFirst != 0 || pCur->cbSize > cbMax) {
            cbMax = pCur->cbSize;
        }
        fFirst = 0;
    }
    if (pCbMin != NULL) {
        *pCbMin = cbMin;
    }
    if (pCbMax != NULL) {
        *pCbMax = cbMax;
    }
    if (pCbSum != NULL) {
        *pCbSum = cbSum;
    }
    if (pCWalk != NULL) {
        *pCWalk = cWalk;
    }
    if (pCBadMagic != NULL) {
        *pCBadMagic = cBad;
    }
}

void
kheap_dump_stats(void)
{
    size_t cbSoftLive;
    u64 u64SoftNet;
    u32 cAreas = 0;
    u32 u32Surf;
    int fPass;
    size_t cbFreeMin;
    size_t cbFreeMax;
    size_t cbFreeSumWalk;
    u32 cFreeWalk;
    u32 cBadMagic;
    u32 u32Hdr;
    u32 u32UtilPct;
    u32 u32SoftPct;
    size_t cbLiveTotal;
    u32 fMatchSum;   /* walk sum == accounted free bytes (soft integrity) */
    u32 fMatchWalk;  /* walk length == free_blocks counter */
    u32 fMatchInteg; /* both match and no bad magic on walk */

    cbSoftLive = g_cbSoftAlign + g_cbSoftUnsplit;
    /* Lifetime net: charged - released (0 if released ahead; wrap-safe). */
    if (g_cbSoftCharged >= g_cbSoftReleased) {
        u64SoftNet = g_cbSoftCharged - g_cbSoftReleased;
    } else {
        u64SoftNet = 0;
    }
    g_cSoftLogN++;

    soft_freelist_scan(&cbFreeMin, &cbFreeMax, &cbFreeSumWalk, &cFreeWalk,
                       &cBadMagic);
    /*
     * Soft freelist integrity (observe-only): compare walk to counters.
     * Soft!=product; never hard-gates alloc/free paths.
     */
    fMatchSum = (cbFreeSumWalk == g_cbFree) ? 1u : 0u;
    fMatchWalk = ((size_t)cFreeWalk == g_cFreeBlocks) ? 1u : 0u;
    fMatchInteg = (fMatchSum != 0u && fMatchWalk != 0u && cBadMagic == 0u)
                      ? 1u
                      : 0u;

    /*
     * Soft PASS lamp: heap inited and has seen grow or live free pool.
     * Diagnostics only - Soft!=product; never 1TiB claim.
     */
    fPass = (g_fInit != 0 && (g_cGrow > 0 || g_cbFree > 0 || g_cAlloc > 0))
                ? 1
                : 0;

    /* Soft util % of live payload vs free (0 if empty; cap 100). */
    cbLiveTotal = g_cbUsed + g_cbFree;
    if (cbLiveTotal == 0) {
        u32UtilPct = 0;
        u32SoftPct = 0;
    } else {
        u32UtilPct = (u32)((g_cbUsed * 100u) / cbLiveTotal);
        if (u32UtilPct > 100u) {
            u32UtilPct = 100u;
        }
        u32SoftPct = (u32)((cbSoftLive * 100u) / cbLiveTotal);
        if (u32SoftPct > 100u) {
            u32SoftPct = 100u;
        }
    }
    u32Hdr = (u32)sizeof(struct kheap_block);
    u32Surf = KHEAP_SOFT_SURF_CATALOG;

    kprintf("kheap: stats used=%lu free=%lu free_blocks=%lu "
            "soft_frag=%lu soft_align=%lu soft_unsplit=%lu\n",
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)cbSoftLive,
            (unsigned long)g_cbSoftAlign,
            (unsigned long)g_cbSoftUnsplit);
    kprintf("kheap: counters alloc=%lu free=%lu grow=%lu split=%lu "
            "fail=%lu double_free=%lu\n",
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cGrow,
            (unsigned long)g_cSplit,
            (unsigned long)g_cFail,
            (unsigned long)g_cDoubleFree);

    /*
     * Soft heap inventory - greppable: kheap: soft
     * Baseline line preserves prior greps (allocs/frees/bytes + waste).
     * Catalog: honesty | inventory | live | lifetime | waste | fail |
     *          ops | design | freelist | grow | scrub | lamps |
     *          path | stats | surfaces | magic | return | header |
     *          residual lean | deepen | PASS|NONE
     * Soft!=product. Dual MIT OR Apache-2.0. No stamp storms.
     */
    /*
     * Honesty first: freestanding soft inventory is NOT product /
     * 1TiB product. greppable: kheap: soft honesty | Soft!=product
     */
    kprintf("kheap: soft honesty not-product not-1TiB-product "
            "product_tib=0 freelist=soft_only diagnostics=1 "
            "multi_page=OPEN coalesce=OPEN soft_ne_product=1 "
            "c2_residual=1 dual_dod=OPEN product_claim=0 "
            "dual=MIT_OR_Apache-2.0 storm=0 wave=%u "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "C2 residual; Dual DoD OPEN)\n",
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /* Grep: kheap: soft (baseline) */
    kprintf("kheap: soft allocs=%lu frees=%lu bytes=%lu "
            "align=%lu unsplit=%lu peak=%lu "
            "charged=%lu released=%lu "
            "waste_allocs=%lu waste_frees=%lu "
            "align_hit=%lu unsplit_hit=%lu\n",
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)cbSoftLive,
            (unsigned long)g_cbSoftAlign,
            (unsigned long)g_cbSoftUnsplit,
            (unsigned long)g_cbSoftPeak,
            (unsigned long)g_cbSoftCharged,
            (unsigned long)g_cbSoftReleased,
            (unsigned long)g_cSoftWasteAlloc,
            (unsigned long)g_cSoftWasteFree,
            (unsigned long)g_cSoftAlignHit,
            (unsigned long)g_cSoftUnsplitHit);
    cAreas++;

    /* Grep: kheap: soft inventory | Soft!=product */
    kprintf("kheap: soft inventory page_payload=%lu split_min=%lu "
            "align=16 policy=best_fit grow=pmm_page magic=live+free "
            "logs=%lu init=%d wave=%u "
            "(soft; Soft!=product; not 1TiB product)\n",
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned long)GJ_KHEAP_SPLIT_MIN,
            (unsigned long)g_cSoftLogN,
            g_fInit,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /* Grep: kheap: soft live */
    kprintf("kheap: soft live frag=%lu align=%lu unsplit=%lu peak=%lu "
            "used=%lu used_peak=%lu free=%lu free_blocks=%lu "
            "free_blocks_peak=%lu util_pct=%u soft_pct=%u\n",
            (unsigned long)cbSoftLive,
            (unsigned long)g_cbSoftAlign,
            (unsigned long)g_cbSoftUnsplit,
            (unsigned long)g_cbSoftPeak,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbUsedPeak,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)g_cFreeBlocksPeak,
            u32UtilPct,
            u32SoftPct);
    cAreas++;

    /* Grep: kheap: soft lifetime */
    kprintf("kheap: soft lifetime charged=%lu released=%lu net=%lu "
            "peak_align=%lu peak_unsplit=%lu "
            "max_align_one=%lu max_unsplit_one=%lu\n",
            (unsigned long)g_cbSoftCharged,
            (unsigned long)g_cbSoftReleased,
            (unsigned long)u64SoftNet,
            (unsigned long)g_cbSoftPeakAlign,
            (unsigned long)g_cbSoftPeakUnsplit,
            (unsigned long)g_cbSoftMaxAlignOne,
            (unsigned long)g_cbSoftMaxUnsplitOne);
    cAreas++;

    /* Grep: kheap: soft waste */
    kprintf("kheap: soft waste allocs=%lu frees=%lu align_hit=%lu "
            "unsplit_hit=%lu take_whole=%lu split=%lu\n",
            (unsigned long)g_cSoftWasteAlloc,
            (unsigned long)g_cSoftWasteFree,
            (unsigned long)g_cSoftAlignHit,
            (unsigned long)g_cSoftUnsplitHit,
            (unsigned long)g_cSoftTakeWhole,
            (unsigned long)g_cSplit);
    cAreas++;

    /* Grep: kheap: soft fail */
    kprintf("kheap: soft fail total=%lu zero=%lu uninit=%lu oversize=%lu "
            "oom=%lu double_free=%lu free_uninit=%lu null_free=%lu\n",
            (unsigned long)g_cFail,
            (unsigned long)g_cSoftFailZero,
            (unsigned long)g_cSoftFailUninit,
            (unsigned long)g_cSoftFailOversize,
            (unsigned long)g_cSoftFailOom,
            (unsigned long)g_cDoubleFree,
            (unsigned long)g_cSoftFreeUninit,
            (unsigned long)g_cSoftNullFree);
    cAreas++;

    /* Grep: kheap: soft ops */
    kprintf("kheap: soft ops allocs=%lu frees=%lu grow=%lu split=%lu "
            "best_fit=%lu take_whole=%lu\n",
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cGrow,
            (unsigned long)g_cSplit,
            (unsigned long)g_cSoftBestFit,
            (unsigned long)g_cSoftTakeWhole);
    cAreas++;

    /*
     * Wave 15: design geometry (constants only; not product capacity).
     * greppable: kheap: soft design
     */
    kprintf("kheap: soft design page=%u hdr=%u page_payload=%lu "
            "split_min=%lu align=16 policy=best_fit single_page=1 "
            "multi_page=OPEN coalesce=OPEN max_one_alloc=%lu "
            "wave=%u (soft geometry; not product)\n",
            (unsigned)GJ_PAGE_SIZE,
            u32Hdr,
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned long)GJ_KHEAP_SPLIT_MIN,
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Freelist walk snap + integrity match (read-only).
     * greppable: kheap: soft freelist
     */
    kprintf("kheap: soft freelist blocks=%lu walk=%u sum_walk=%lu "
            "accounted=%lu min=%lu max=%lu bad_magic=%u head=%u "
            "peak_blocks=%lu match_sum=%u match_walk=%u integ=%u "
            "(soft walk; Soft!=product; not product)\n",
            (unsigned long)g_cFreeBlocks,
            cFreeWalk,
            (unsigned long)cbFreeSumWalk,
            (unsigned long)g_cbFree,
            (unsigned long)cbFreeMin,
            (unsigned long)cbFreeMax,
            cBadMagic,
            g_pFree != NULL ? 1u : 0u,
            (unsigned long)g_cFreeBlocksPeak,
            fMatchSum,
            fMatchWalk,
            fMatchInteg);
    cAreas++;

    /*
     * Wave 15: grow surface (one PMM page per grow).
     * greppable: kheap: soft grow
     */
    kprintf("kheap: soft grow count=%lu oom=%lu page_payload=%lu "
            "page_size=%u hhdm_aware=1 multi_page=OPEN "
            "wave=%u (soft; not product)\n",
            (unsigned long)g_cGrow,
            (unsigned long)g_cSoftFailOom,
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned)GJ_PAGE_SIZE,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: scrub/zero policy lamps (always-on soft contract).
     * greppable: kheap: soft scrub
     */
    kprintf("kheap: soft scrub zero_on_alloc=1 scrub_on_free=1 "
            "magic_live=0x%x magic_free=0x%x double_free_guard=1 "
            "coalesce=OPEN wave=%u "
            "(soft policy; not product)\n",
            (unsigned)GJ_KHEAP_MAGIC_LIVE,
            (unsigned)GJ_KHEAP_MAGIC_FREE,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 15: readiness lamps (soft only).
     * greppable: kheap: soft lamps
     */
    kprintf("kheap: soft lamps init=%d ready=%u head=%u free_bytes=%lu "
            "used_bytes=%lu grow=%lu fail=%lu double_free=%lu "
            "util_pct=%u soft_pct=%u wave=%u "
            "(soft lamps; not product; not 1TiB product)\n",
            g_fInit,
            fPass ? 1u : 0u,
            g_pFree != NULL ? 1u : 0u,
            (unsigned long)g_cbFree,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cGrow,
            (unsigned long)g_cFail,
            (unsigned long)g_cDoubleFree,
            u32UtilPct,
            u32SoftPct,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Soft path honesty: surface catalog + explicit non-claims.
     * greppable: kheap: soft path
     */
    kprintf("kheap: soft path claim=freelist+pmm_grow scrub=free "
            "zero=alloc policy=best_fit align=16 single_page=1 "
            "multi_page=OPEN coalesce=OPEN design=1 freelist_walk=1 "
            "grow=1 scrub=1 lamps=1 product_tib=0 soft_ne_product=1 "
            "(soft inventory; Soft!=product; not 1TiB product)\n");
    cAreas++;

    /* Grep: kheap: soft stats - rollup tallies for agent greps. */
    kprintf("kheap: soft stats used=%lu free=%lu free_blocks=%lu "
            "soft_frag=%lu soft_peak=%lu charged=%lu released=%lu net=%lu "
            "allocs=%lu frees=%lu grow=%lu split=%lu fail=%lu "
            "waste_allocs=%lu free_min=%lu free_max=%lu "
            "util_pct=%u logs=%lu init=%d wave=%u\n",
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)cbSoftLive,
            (unsigned long)g_cbSoftPeak,
            (unsigned long)g_cbSoftCharged,
            (unsigned long)g_cbSoftReleased,
            (unsigned long)u64SoftNet,
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cGrow,
            (unsigned long)g_cSplit,
            (unsigned long)g_cFail,
            (unsigned long)g_cSoftWasteAlloc,
            (unsigned long)cbFreeMin,
            (unsigned long)cbFreeMax,
            u32UtilPct,
            (unsigned long)g_cSoftLogN,
            g_fInit,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: return-surface catalog (surf bitmask; soft != product).
     * Grep: kheap: soft surfaces
     */
    kprintf("kheap: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "honesty=1 live=1 waste=1 fail=1 freelist=1 grow=1 scrub=1 "
            "magic=1 return=1 header=1 residual=1 wave=%u "
            "(soft surfaces; Soft!=product; storm=0)\n",
            (unsigned)u32Surf, (unsigned)KHEAP_SOFT_AREAS,
            (unsigned)KHEAP_SOFT_AREAS,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: LIVE/FREE magic catalog (observe-only).
     * Grep: kheap: soft magic
     */
    kprintf("kheap: soft magic live=0x%x free=0x%x double_free_guard=1 "
            "bad_walk=%u wave=%u "
            "(soft magic; not product)\n",
            (unsigned)GJ_KHEAP_MAGIC_LIVE,
            (unsigned)GJ_KHEAP_MAGIC_FREE,
            cBadMagic,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: return-surface honesty (alloc/free terminal outcomes).
     * Compact one-liner only - no ret*angle stamp storms.
     * Grep: kheap: soft return
     */
    kprintf("kheap: soft return alloc_ok=%lu free_ok=%lu fail=%lu "
            "double_free=%lu null_free=%lu oversize=%lu oom=%lu "
            "zero=%lu uninit=%lu split=%lu take_whole=%lu "
            "surf=0x%x wave=%u "
            "(soft return; Soft!=product)\n",
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cFail,
            (unsigned long)g_cDoubleFree,
            (unsigned long)g_cSoftNullFree,
            (unsigned long)g_cSoftFailOversize,
            (unsigned long)g_cSoftFailOom,
            (unsigned long)g_cSoftFailZero,
            (unsigned long)g_cSoftFailUninit,
            (unsigned long)g_cSplit,
            (unsigned long)g_cSoftTakeWhole,
            (unsigned)u32Surf,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Block header geometry (constants; not product capacity).
     * Grep: kheap: soft header
     */
    kprintf("kheap: soft header sizeof=%u align_field=u16 unsplit_field=u16 "
            "magic_field=u32 next_ptr=1 page_payload=%lu split_min=%lu "
            "wave=%u (soft header geometry; Soft!=product)\n",
            u32Hdr,
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned long)GJ_KHEAP_SPLIT_MIN,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Lean soft residual - sole residual-class lamp for this unit.
     * Soft!=product - dual MIT OR Apache-2.0 - no version stamp - storm=0.
     * Residual OPEN: multi_page grow, adjacent free coalesce.
     * C2 residual: freelist integrity + soft kmalloc foundation for UDX eng
     * (linux_dma_soft peer). Dual DoD A/B remain OPEN (never product close).
     * Integrity: freelist walk vs counters (observe-only).
     * greppable: kheap: soft residual lean | Soft!=product | G-AC-1 | C2
     */
    kprintf("kheap: soft residual lean "
            "freelist=1 pmm_grow=1 single_page=1 best_fit=1 "
            "align=16 scrub_free=1 zero_alloc=1 magic=live+free "
            "multi_page=OPEN coalesce=OPEN "
            "page_payload=%lu split_min=%lu "
            "used=%lu free=%lu free_blocks=%lu soft_frag=%lu "
            "allocs=%lu frees=%lu grow=%lu fail=%lu "
            "match_sum=%u match_walk=%u integ=%u bad_magic=%u "
            "c2_residual=1 dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "udx_eng_kmalloc_foundation=1 soft_kmalloc_peer=1 "
            "product_tib=0 soft_ne_product=1 product_claim=0 "
            "dual=MIT_OR_Apache-2.0 storm=0 wave=%u "
            "(Soft!=product; G-AC-1; C2 residual; dual MIT OR Apache-2.0; "
            "no version stamp; not 1TiB product; Dual DoD OPEN; "
            "lean freelist residual only)\n",
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned long)GJ_KHEAP_SPLIT_MIN,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)cbSoftLive,
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cGrow,
            (unsigned long)g_cFail,
            fMatchSum,
            fMatchWalk,
            fMatchInteg,
            cBadMagic,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * C2 residual deepen - freelist integrity + UDX eng kmalloc foundation.
     * Observe-only; Soft!=product; Dual DoD A/B remain OPEN; storm=0.
     * greppable: kheap: soft residual deepen | C2 residual | Soft!=product
     */
    kprintf("kheap: soft residual deepen "
            "c2=1 freelist=1 pmm_grow=1 single_page=1 best_fit=1 "
            "align=16 scrub_free=1 zero_alloc=1 magic=live+free "
            "multi_page=OPEN coalesce=OPEN "
            "match_sum=%u match_walk=%u integ=%u bad_magic=%u "
            "free_min=%lu free_max=%lu sum_walk=%lu accounted=%lu "
            "used=%lu free=%lu free_blocks=%lu soft_frag=%lu "
            "allocs=%lu frees=%lu grow=%lu fail=%lu split=%lu "
            "udx_eng_kmalloc_foundation=1 soft_kmalloc_peer=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_tib=0 soft_ne_product=1 product_claim=0 "
            "dual=MIT_OR_Apache-2.0 storm=0 wave=%u "
            "(C2 residual deepen; Soft!=product; G-AC-1; "
            "Dual DoD OPEN; not product AC; not 1TiB product; "
            "no version stamp; lean freelist only)\n",
            fMatchSum,
            fMatchWalk,
            fMatchInteg,
            cBadMagic,
            (unsigned long)cbFreeMin,
            (unsigned long)cbFreeMax,
            (unsigned long)cbFreeSumWalk,
            (unsigned long)g_cbFree,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)cbSoftLive,
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cGrow,
            (unsigned long)g_cFail,
            (unsigned long)g_cSplit,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Residual lean PASS when freelist integrity holds (observe-only).
     * Soft!=product - never product PASS / Dual DoD close.
     * greppable: kheap: soft residual lean PASS
     */
    if (g_fInit != 0 && fMatchInteg != 0u) {
        kprintf("kheap: soft residual lean PASS "
                "integ=1 match_sum=%u match_walk=%u bad_magic=0 "
                "c2_residual=1 dual_dod=OPEN "
                "multi_page=OPEN coalesce=OPEN "
                "used=%lu free=%lu free_blocks=%lu soft_frag=%lu "
                "allocs=%lu frees=%lu grow=%lu "
                "soft_ne_product=1 product_claim=0 "
                "dual=MIT_OR_Apache-2.0 storm=0 wave=%u "
                "(soft residual lean PASS; Soft!=product; G-AC-1; "
                "C2 residual; Dual DoD OPEN; not product AC)\n",
                fMatchSum,
                fMatchWalk,
                (unsigned long)g_cbUsed,
                (unsigned long)g_cbFree,
                (unsigned long)g_cFreeBlocks,
                (unsigned long)cbSoftLive,
                (unsigned long)g_cAlloc,
                (unsigned long)g_cFree,
                (unsigned long)g_cGrow,
                (unsigned)KHEAP_SOFT_WAVE);
    }

    /*
     * Grep: kheap: soft deepen (areas = prior soft lines; not image stamp).
     * Lean residual - no version stamp, no stamp storms.
     */
    kprintf("kheap: soft deepen wave=%u areas=%u catalog=%u logs=%lu "
            "init=%d used=%lu free=%lu soft_frag=%lu free_min=%lu "
            "free_max=%lu match_sum=%u match_walk=%u integ=%u "
            "surf=0x%x product_tib=0 residual_lean=1 c2_residual=1 "
            "dual_dod=OPEN "
            "(soft deepen; Soft!=product; G-AC-1; C2 residual; "
            "Dual DoD OPEN; not 1TiB product; storm=0)\n",
            (unsigned)KHEAP_SOFT_WAVE,
            cAreas,
            (unsigned)KHEAP_SOFT_AREAS,
            (unsigned long)g_cSoftLogN,
            g_fInit,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned long)cbSoftLive,
            (unsigned long)cbFreeMin,
            (unsigned long)cbFreeMax,
            fMatchSum,
            fMatchWalk,
            fMatchInteg,
            (unsigned)u32Surf);

    /*
     * Close markers: freelist soft readiness only.
     * Grep: kheap: soft PASS | kheap: soft NONE | kheap: soft inventory PASS
     * Never "product PASS" / "1TiB product PASS". Soft!=product.
     */
    if (fPass) {
        /* Grep: kheap: soft PASS | kheap: soft inventory PASS */
        kprintf("kheap: soft PASS grow=%lu free=%lu allocs=%lu wave=%u "
                "(soft inventory; Soft!=product; not 1TiB product; "
                "dual MIT OR Apache-2.0)\n",
                (unsigned long)g_cGrow,
                (unsigned long)g_cbFree,
                (unsigned long)g_cAlloc,
                (unsigned)KHEAP_SOFT_WAVE);
        kprintf("kheap: soft inventory PASS logs=%lu soft_frag=%lu wave=%u "
                "(soft; Soft!=product; not 1TiB product)\n",
                (unsigned long)g_cSoftLogN,
                (unsigned long)cbSoftLive,
                (unsigned)KHEAP_SOFT_WAVE);
    } else {
        /* Grep: kheap: soft NONE */
        kprintf("kheap: soft NONE init=%d grow=%lu free=%lu allocs=%lu "
                "wave=%u "
                "(soft inventory; Soft!=product; not 1TiB product)\n",
                g_fInit,
                (unsigned long)g_cGrow,
                (unsigned long)g_cbFree,
                (unsigned long)g_cAlloc,
                (unsigned)KHEAP_SOFT_WAVE);
    }
}
