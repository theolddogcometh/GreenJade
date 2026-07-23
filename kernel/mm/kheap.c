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
 * Counters are live (subtracted on free), not lifetime totals.
 *
 * Greppable: kheap: init | kheap: stats | kheap: counters
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
static size_t              g_cbSoftAlign;
static size_t              g_cbSoftUnsplit;
static u64                 g_cAlloc;
static u64                 g_cFree;
static u64                 g_cGrow;
static u64                 g_cSplit;
static u64                 g_cFail;
static u64                 g_cDoubleFree;
static int                 g_fInit;

void
kheap_init(void)
{
    g_pFree = NULL;
    g_cbUsed = 0;
    g_cbFree = 0;
    g_cFreeBlocks = 0;
    g_cbSoftAlign = 0;
    g_cbSoftUnsplit = 0;
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

    if (!g_fInit || cb == 0) {
        g_cFail++;
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
            g_cbUsed += pBest->cbSize;
            g_cAlloc++;
            pOut = payload_from_block(pBest);
            memset(pOut, 0, pBest->cbSize);
            return pOut;
        }
        if (grow() != GJ_OK) {
            g_cFail++;
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

    if (p == NULL || !g_fInit) {
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
    kprintf("kheap: stats used=%lu free=%lu free_blocks=%lu "
            "soft_frag=%lu soft_align=%lu soft_unsplit=%lu\n",
            (unsigned long)g_cbUsed,
            (unsigned long)g_cbFree,
            (unsigned long)g_cFreeBlocks,
            (unsigned long)(g_cbSoftAlign + g_cbSoftUnsplit),
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
}
