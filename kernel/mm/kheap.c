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
 * Soft product inventory (Wave 11 exclusive deepen; this unit only):
 *   - Live soft: align / unsplit / frag / peak / used / free / free_blocks
 *   - Lifetime: charged / released / net / peak components / max-one
 *   - Waste events: waste_allocs/frees, align/unsplit hits, take_whole
 *   - Fail taxonomy: zero / uninit / oversize / oom / double_free
 *   - Ops: allocs / frees / grow / split / best_fit / null_free
 *   greppable: "kheap: soft …"
 *   Never hard-gates; diagnostics only (wrap OK).
 *
 * Greppable:
 *   kheap: init
 *   kheap: stats …
 *   kheap: counters …
 *   kheap: soft …            (baseline allocs/frees/bytes + waste)
 *   kheap: soft inventory …
 *   kheap: soft live …
 *   kheap: soft lifetime …
 *   kheap: soft waste …
 *   kheap: soft fail …
 *   kheap: soft ops …
 *   kheap: soft path …
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/vmm.h>

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

void
kheap_dump_stats(void)
{
    size_t cbSoftLive;
    u64 u64SoftNet;

    cbSoftLive = g_cbSoftAlign + g_cbSoftUnsplit;
    /* Lifetime net: charged - released (0 if released ahead; wrap-safe). */
    if (g_cbSoftCharged >= g_cbSoftReleased) {
        u64SoftNet = g_cbSoftCharged - g_cbSoftReleased;
    } else {
        u64SoftNet = 0;
    }
    g_cSoftLogN++;

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
     * Soft heap inventory deepen (Wave 11) — greppable: kheap: soft
     * Baseline line preserves prior greps (allocs/frees/bytes + waste).
     * Catalog sub-lines: inventory | live | lifetime | waste | fail | ops | path
     */
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

    /* Grep: kheap: soft inventory */
    kprintf("kheap: soft inventory page_payload=%lu split_min=%lu "
            "align=16 policy=best_fit grow=pmm_page magic=live+free "
            "logs=%lu\n",
            (unsigned long)GJ_KHEAP_PAGE_PAYLOAD,
            (unsigned long)GJ_KHEAP_SPLIT_MIN,
            (unsigned long)g_cSoftLogN);

    /* Grep: kheap: soft live */
    kprintf("kheap: soft live frag=%lu align=%lu unsplit=%lu peak=%lu "
            "used=%lu used_peak=%lu free=%lu free_blocks=%lu "
            "free_blocks_peak=%lu\n",
            (unsigned long)cbSoftLive,
            (unsigned long)g_cbSoftAlign,
            (unsigned long)g_cbSoftUnsplit,
            (unsigned long)g_cbSoftPeak,
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbUsedPeak,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)g_cFreeBlocksPeak);

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

    /* Grep: kheap: soft waste */
    kprintf("kheap: soft waste allocs=%lu frees=%lu align_hit=%lu "
            "unsplit_hit=%lu take_whole=%lu split=%lu\n",
            (unsigned long)g_cSoftWasteAlloc,
            (unsigned long)g_cSoftWasteFree,
            (unsigned long)g_cSoftAlignHit,
            (unsigned long)g_cSoftUnsplitHit,
            (unsigned long)g_cSoftTakeWhole,
            (unsigned long)g_cSplit);

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

    /* Grep: kheap: soft ops */
    kprintf("kheap: soft ops allocs=%lu frees=%lu grow=%lu split=%lu "
            "best_fit=%lu take_whole=%lu\n",
            (unsigned long)g_cAlloc,
            (unsigned long)g_cFree,
            (unsigned long)g_cGrow,
            (unsigned long)g_cSplit,
            (unsigned long)g_cSoftBestFit,
            (unsigned long)g_cSoftTakeWhole);

    /* Grep: kheap: soft path */
    kprintf("kheap: soft path claim=freelist+pmm_grow scrub=free "
            "zero=alloc policy=best_fit (soft inventory; not bar3)\n");
}
