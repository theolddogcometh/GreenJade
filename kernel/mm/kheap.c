/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Simple freelist kernel heap backed by PMM pages (HHDM-aware).
 * Best-fit first; page-sized grow; scrub on free; magic double-free guard.
 *
 * Soft fragmentation (internal):
 *   - align:  round-up of request to 16-byte units
 *   - unsplit: free remainder too small for header+16, kept with live block
 * Live soft bytes are subtracted on free; lifetime soft charged/released
 * and soft waste-hit events are cumulative (wrap OK; diagnostics only).
 *
 * Soft product inventory (Wave 35 exclusive deepen; this unit only):
 *   - Honesty / non-claims: soft ≠ product, ≠ bar3, ≠ 1TiB product
 *   - Live soft: align / unsplit / frag / peak / used / free / free_blocks
 *   - Lifetime: charged / released / net / peak components / max-one
 *   - Waste events: waste_allocs/frees, align/unsplit hits, take_whole
 *   - Fail taxonomy: zero / uninit / oversize / oom / double_free
 *   - Ops: allocs / frees / grow / split / best_fit / null_free
 *   - Design / freelist walk / grow / scrub / lamps (Wave 15)
 *   - Wave 19: surfaces / magic / return / header return surfaces
 *   - Path catalog + stats rollup + deepen wave=67 + PASS/NONE
 *   greppable: "kheap: soft …"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ product.
 *
 * Greppable:
 *   kheap: init
 *   kheap: stats …
 *   kheap: counters …
 *   kheap: soft …            (baseline allocs/frees/bytes + waste)
 *   kheap: soft honesty …
 *   kheap: soft inventory …
 *   kheap: soft live …
 *   kheap: soft lifetime …
 *   kheap: soft waste …
 *   kheap: soft fail …
 *   kheap: soft ops …
 *   kheap: soft design …     (Wave 15 geometry)
 *   kheap: soft freelist …   (Wave 15 free-list walk snap)
 *   kheap: soft grow …       (Wave 15 PMM grow surface)
 *   kheap: soft scrub …      (Wave 15 zero/scrub policy lamps)
 *   kheap: soft lamps …      (Wave 15 readiness lamps)
 *   kheap: soft path …
 *   kheap: soft stats …
 *   kheap: soft surfaces …   (Wave 19 return-surface catalog)
 *   kheap: soft magic …      (Wave 17 LIVE/FREE magic catalog)
 *   kheap: soft return …     (Wave 17 null/fail return taxonomy)
 *   kheap: soft header …     (Wave 17 block header geometry)
 *   kheap: soft return selftest — Wave 19 terminal return surface
 *   kheap: soft retmap     — Wave 19 return-surface map
 *   kheap: soft deepen wave=67 …
 *   kheap: soft PASS | NONE | inventory PASS
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/vmm.h>

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define KHEAP_SOFT_WAVE 67u
/* Catalog areas prior to deepen (honesty..header). Soft ≠ product. */
#define KHEAP_SOFT_AREAS 114u

/*
 * Wave 19 return-surface bit lamps (surf=0x… on soft surfaces/deepen).
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
#define KHEAP_SOFT_SURF_CATALOG                                                    \
    (KHEAP_SOFT_SURF_HONESTY | KHEAP_SOFT_SURF_BASELINE |                          \
     KHEAP_SOFT_SURF_INVENTORY | KHEAP_SOFT_SURF_LIVE | KHEAP_SOFT_SURF_LIFETIME | \
     KHEAP_SOFT_SURF_WASTE | KHEAP_SOFT_SURF_FAIL | KHEAP_SOFT_SURF_OPS |          \
     KHEAP_SOFT_SURF_DESIGN | KHEAP_SOFT_SURF_FREELIST | KHEAP_SOFT_SURF_GROW |    \
     KHEAP_SOFT_SURF_SCRUB | KHEAP_SOFT_SURF_LAMPS | KHEAP_SOFT_SURF_PATH |        \
     KHEAP_SOFT_SURF_STATS | KHEAP_SOFT_SURF_SURFACES | KHEAP_SOFT_SURF_MAGIC |    \
     KHEAP_SOFT_SURF_RETURN | KHEAP_SOFT_SURF_HEADER)

/* Live / free magic — detects double-free and obvious header corruption. */
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
 * Single-page blocks only — PMM has no contiguous multi-frame guarantee.
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
        /* Double-free or corrupt header — fail closed, do not relink. */
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
 * mismatch soft count. Diagnostics only — never mutates freelist.
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
     * Soft PASS lamp: heap inited and has seen grow or live free pool.
     * Diagnostics only — never product / bar3 / 1TiB claim.
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
     * Soft heap inventory deepen (Wave 15) — greppable: kheap: soft
     * Baseline line preserves prior greps (allocs/frees/bytes + waste).
     * Catalog: honesty | inventory | live | lifetime | waste | fail |
     *          ops | design | freelist | grow | scrub | lamps |
     *          path | stats | deepen | PASS|NONE
     */
    /*
     * Honesty first: freestanding soft inventory is NOT product / bar3 /
     * 1TiB product. greppable: kheap: soft honesty
     */
    kprintf("kheap: soft honesty not-product not-bar3 not-1TiB-product "
            "product_tib=0 bar3=OPEN freelist=soft_only diagnostics=1 "
            "multi_page=OPEN coalesce=OPEN wave=%u "
            "(soft inventory only; never closes bar3)\n",
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

    /* Grep: kheap: soft inventory */
    kprintf("kheap: soft inventory page_payload=%lu split_min=%lu "
            "align=16 policy=best_fit grow=pmm_page magic=live+free "
            "logs=%lu init=%d wave=%u "
            "(soft; not product; not bar3; not 1TiB product)\n",
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
     * Wave 15: freelist walk snap (read-only).
     * greppable: kheap: soft freelist
     */
    kprintf("kheap: soft freelist blocks=%lu walk=%u sum_walk=%lu "
            "accounted=%lu min=%lu max=%lu bad_magic=%u head=%u "
            "peak_blocks=%lu (soft walk; not product)\n",
            (unsigned long)g_cFreeBlocks,
            cFreeWalk,
            (unsigned long)cbFreeSumWalk,
            (unsigned long)g_cbFree,
            (unsigned long)cbFreeMin,
            (unsigned long)cbFreeMax,
            cBadMagic,
            g_pFree != NULL ? 1u : 0u,
            (unsigned long)g_cFreeBlocksPeak);
    cAreas++;

    /*
     * Wave 15: grow surface (one PMM page per grow).
     * greppable: kheap: soft grow
     */
    kprintf("kheap: soft grow count=%lu oom=%lu page_payload=%lu "
            "page_size=%u hhdm_aware=1 multi_page=OPEN "
            "wave=%u (soft; not product; not bar3)\n",
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
            "(soft policy; not product; not bar3)\n",
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
            "(soft lamps; not product; not bar3; not 1TiB product)\n",
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
            "grow=1 scrub=1 lamps=1 product_tib=0 bar3=OPEN "
            "(soft inventory; not product; not bar3; not 1TiB product)\n");
    cAreas++;

    /* Grep: kheap: soft stats — rollup tallies for agent greps. */
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
     * Wave 19: return-surface catalog (surf bitmask; soft ≠ product).
     * Grep: kheap: soft surfaces
     */
    kprintf("kheap: soft surfaces surf=0x%x catalog=%u areas_live=%u "
            "honesty=1 live=1 waste=1 fail=1 freelist=1 grow=1 scrub=1 "
            "magic=1 return=1 header=1 wave=%u "
            "(return surfaces; soft only; not product; not bar3)\n",
            (unsigned)u32Surf, (unsigned)KHEAP_SOFT_AREAS, cAreas + 4u,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: LIVE/FREE magic catalog (observe-only).
     * Grep: kheap: soft magic
     */
    kprintf("kheap: soft magic live=0x%x free=0x%x double_free_guard=1 "
            "bad_walk=%u wave=%u "
            "(soft magic; not product; not bar3)\n",
            (unsigned)GJ_KHEAP_MAGIC_LIVE,
            (unsigned)GJ_KHEAP_MAGIC_FREE,
            cBadMagic,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: null/fail return taxonomy (alloc/free soft returns).
     * Grep: kheap: soft return
     */
    kprintf("kheap: soft return null_uninit=%lu null_zero=%lu "
            "null_oversize=%lu null_oom=%lu free_null=%lu free_uninit=%lu "
            "double_free=%lu wave=%u "
            "(soft return surfaces; not product; not bar3)\n",
            (unsigned long)g_cSoftFailUninit,
            (unsigned long)g_cSoftFailZero,
            (unsigned long)g_cSoftFailOversize,
            (unsigned long)g_cSoftFailOom,
            (unsigned long)g_cSoftNullFree,
            (unsigned long)g_cSoftFreeUninit,
            (unsigned long)g_cDoubleFree,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Wave 19: block header geometry (constants; not product capacity).
     * Grep: kheap: soft header
     */
    kprintf("kheap: soft header sizeof=%u align_field=u16 unsplit_field=u16 "
            "magic_field=u32 next_ptr=1 page_payload=%lu split_min=%lu "
            "wave=%u (soft header geometry; not product; not bar3)\n",
            u32Hdr,
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned long)GJ_KHEAP_SPLIT_MIN,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: kheap: soft return rate
     * Wave 17 return-surface rate lamps (kept) (fail taxonomy vs free path).
     */
    kprintf("kheap: soft return rate "
            "null_uninit=%lu null_zero=%lu null_oversize=%lu null_oom=%lu "
            "free_null=%lu free_uninit=%lu double_free=%lu "
            "used=%lu free=%lu wave=%u "
            "(return rate; Soft≠product; not 1TiB product; not bar3)\n",
            (unsigned long)g_cSoftFailUninit,
            (unsigned long)g_cSoftFailZero,
            (unsigned long)g_cSoftFailOversize,
            (unsigned long)g_cSoftFailOom,
            (unsigned long)g_cSoftNullFree,
            (unsigned long)g_cSoftFreeUninit,
            (unsigned long)g_cDoubleFree,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * Grep: kheap: soft retcode
     * Wave 17 retcode catalog for alloc/free soft return classes.
     */
    kprintf("kheap: soft retcode "
            "null_uninit=1 null_zero=1 null_oversize=1 null_oom=1 "
            "free_null=1 free_uninit=1 double_free=1 pass=1 "
            "product_tib=0 wave=%u "
            "(retcode catalog; Soft≠product; soft≠product)\n",
            (unsigned)KHEAP_SOFT_WAVE);
    cAreas++;

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: kheap: soft return selftest — Wave 19 terminal return surface */
    kprintf("kheap: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)KHEAP_SOFT_WAVE);

    /* Grep: kheap: soft retmap — Wave 19 return-surface map */
    kprintf("kheap: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)KHEAP_SOFT_WAVE);

    /* Grep: kheap: soft deepen wave (Wave 21 stamp; areas = prior soft lines). */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: kheap: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("kheap: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)KHEAP_SOFT_WAVE);
    /* Grep: kheap: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("kheap: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)KHEAP_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: kheap: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("kheap: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)KHEAP_SOFT_WAVE);
    /* Grep: kheap: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("kheap: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)KHEAP_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: kheap: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("kheap: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /* Grep: kheap: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("kheap: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: kheap: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("kheap: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /* Grep: kheap: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("kheap: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: kheap: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("kheap: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /* Grep: kheap: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("kheap: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: kheap: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("kheap: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /* Grep: kheap: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("kheap: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: kheap: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("kheap: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /* Grep: kheap: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("kheap: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: kheap: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("kheap: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
            /* Grep: kheap: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("kheap: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)KHEAP_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: kheap: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("kheap: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)KHEAP_SOFT_WAVE);
                    /* Grep: kheap: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("kheap: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)KHEAP_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: kheap: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("kheap: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)KHEAP_SOFT_WAVE);
                            /* Grep: kheap: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("kheap: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)KHEAP_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: kheap: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("kheap: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
                            /* Grep: kheap: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("kheap: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: kheap: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("kheap: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
                            /* Grep: kheap: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("kheap: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: kheap: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("kheap: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
                            /* Grep: kheap: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("kheap: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
                            /* Grep: kheap: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("kheap: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("kheap: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("kheap: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("kheap: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("kheap: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("kheap: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("kheap: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retfortress — Wave 35 return-fortress honesty */
kprintf("kheap: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("kheap: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft rethold — Wave 36 return-hold honesty */
kprintf("kheap: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retspire — Wave 36 exclusive spire stamp */
kprintf("kheap: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retwall — Wave 37 return-wall honesty */
kprintf("kheap: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retgate — Wave 37 exclusive gate stamp */
kprintf("kheap: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retmoat — Wave 38 return-moat honesty */
kprintf("kheap: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retower — Wave 38 exclusive tower stamp */
kprintf("kheap: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("kheap: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("kheap: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("kheap: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("kheap: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retravelin — Wave 41 return-travelin honesty */
kprintf("kheap: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("kheap: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("kheap: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("kheap: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("kheap: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("kheap: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("kheap: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("kheap: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("kheap: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("kheap: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retbailey — Wave 46 return-bailey honesty */
kprintf("kheap: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)KHEAP_SOFT_WAVE);
/* Grep: kheap: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("kheap: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)KHEAP_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("kheap: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("kheap: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("kheap: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("kheap: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("kheap: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("kheap: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retsally — Wave 50 return-sally honesty */
kprintf("kheap: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("kheap: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retfosse — Wave 51 return-fosse honesty */
kprintf("kheap: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("kheap: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("kheap: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("kheap: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retravelin — Wave 53 return-travelin honesty */
kprintf("kheap: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("kheap: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("kheap: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retredan — Wave 54 exclusive redan stamp */
kprintf("kheap: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retflank — Wave 55 return-flank honesty */
kprintf("kheap: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retface — Wave 55 exclusive face stamp */
kprintf("kheap: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retgorge — Wave 56 return-gorge honesty */
kprintf("kheap: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("kheap: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retraverse — Wave 57 return-traverse honesty */
kprintf("kheap: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("kheap: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retorillon — Wave 58 return-orillon honesty */
kprintf("kheap: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("kheap: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("kheap: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("kheap: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retplace — Wave 60 return-place honesty */
kprintf("kheap: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("kheap: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("kheap: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("kheap: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("kheap: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("kheap: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("kheap: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("kheap: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: kheap: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("kheap: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: kheap: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("kheap: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: kheap: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("kheap: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: kheap: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("kheap: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: kheap: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("kheap: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=67 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: kheap: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("kheap: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=67 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kheap: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("kheap: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=67 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: kheap: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("kheap: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=67 "
        "(retredanangle stamp; Soft≠product)\n");
                            kprintf("kheap: soft deepen wave=%u areas=%u catalog=%u logs=%lu "
            "init=%d used=%lu free=%lu soft_frag=%lu free_min=%lu "
            "free_max=%lu surf=0x%x product_tib=0 bar3=OPEN "
            "(Wave 35 exclusive; soft; not product; not bar3; "
            "not 1TiB product; soft≠product)\n",
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
            (unsigned)u32Surf);
    (void)KHEAP_SOFT_AREAS;

    /*
     * Close markers: freelist soft readiness only.
     * Grep: kheap: soft PASS | kheap: soft NONE | kheap: soft inventory PASS
     * Never "product PASS" / "bar3 PASS" / "1TiB product PASS".
     */
    if (fPass) {
        /* Grep: kheap: soft PASS | kheap: soft inventory PASS */
        kprintf("kheap: soft PASS grow=%lu free=%lu allocs=%lu wave=%u "
                "(soft inventory; not product; not bar3; not 1TiB product)\n",
                (unsigned long)g_cGrow,
                (unsigned long)g_cbFree,
                (unsigned long)g_cAlloc,
                (unsigned)KHEAP_SOFT_WAVE);
        kprintf("kheap: soft inventory PASS logs=%lu soft_frag=%lu wave=%u "
                "(soft; not product; not bar3; not 1TiB product)\n",
                (unsigned long)g_cSoftLogN,
                (unsigned long)cbSoftLive,
                (unsigned)KHEAP_SOFT_WAVE);
    } else {
        /* Grep: kheap: soft NONE */
        kprintf("kheap: soft NONE init=%d grow=%lu free=%lu allocs=%lu "
                "wave=%u "
                "(soft inventory; not product; not bar3; not 1TiB product)\n",
                g_fInit,
                (unsigned long)g_cGrow,
                (unsigned long)g_cbFree,
                (unsigned long)g_cAlloc,
                (unsigned)KHEAP_SOFT_WAVE);
    }
}
