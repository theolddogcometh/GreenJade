/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Simple freelist kernel heap backed by PMM pages (HHDM-aware).
 * Best-fit first; page-sized grow; scrub on free; magic double-free guard.
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
    size_t               cbSize;   /* usable payload bytes */
    u32                  u32Magic; /* LIVE or FREE */
    u32                  u32Pad;
    struct kheap_block  *pNext;    /* free-list link when free */
};

/* Usable payload per grown page (one header at page base). */
#define GJ_KHEAP_PAGE_PAYLOAD \
    ((size_t)GJ_PAGE_SIZE - sizeof(struct kheap_block))

static struct kheap_block *g_pFree;
static size_t              g_cbUsed;
static int                 g_fInit;

void
kheap_init(void)
{
    g_pFree = NULL;
    g_cbUsed = 0;
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
    pBlk->u32Pad = 0;
    pBlk->pNext = g_pFree;
    g_pFree = pBlk;
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
    void *pOut;

    if (!g_fInit || cb == 0) {
        return NULL;
    }
    cbNeed = align_up(cb);
    /*
     * One header + payload must fit a single grown page. Larger requests
     * need a future multi-page path (contiguous frames or chained slabs).
     */
    if (cbNeed > GJ_KHEAP_PAGE_PAYLOAD) {
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
            /* Split if leftover holds another header + 16-byte payload. */
            if (pBest->cbSize >= cbNeed + sizeof(struct kheap_block) + 16u) {
                struct kheap_block *pSplit;
                size_t cbLeft;

                cbLeft = pBest->cbSize - cbNeed - sizeof(struct kheap_block);
                pSplit = (struct kheap_block *)((u8 *)payload_from_block(pBest) +
                                                cbNeed);
                pSplit->cbSize = cbLeft;
                pSplit->u32Magic = GJ_KHEAP_MAGIC_FREE;
                pSplit->u32Pad = 0;
                pSplit->pNext = pBest->pNext;
                pBest->cbSize = cbNeed;
                pBest->pNext = pSplit;
            }
            /* Unlink best from free list */
            if (pBestPrev != NULL) {
                pBestPrev->pNext = pBest->pNext;
            } else {
                g_pFree = pBest->pNext;
            }
            pBest->u32Magic = GJ_KHEAP_MAGIC_LIVE;
            pBest->pNext = NULL;
            g_cbUsed += pBest->cbSize;
            pOut = payload_from_block(pBest);
            memset(pOut, 0, pBest->cbSize);
            return pOut;
        }
        if (grow() != GJ_OK) {
            return NULL;
        }
    }
}

void
kheap_free(void *p)
{
    struct kheap_block *pBlk;

    if (p == NULL || !g_fInit) {
        return;
    }
    pBlk = block_from_payload(p);
    if (pBlk->u32Magic != GJ_KHEAP_MAGIC_LIVE) {
        /* Double-free or corrupt header — fail closed, do not relink. */
        return;
    }
    if (g_cbUsed >= pBlk->cbSize) {
        g_cbUsed -= pBlk->cbSize;
    } else {
        g_cbUsed = 0;
    }
    /* Scrub payload so use-after-free does not leak secrets. */
    memset(payload_from_block(pBlk), 0, pBlk->cbSize);
    pBlk->u32Magic = GJ_KHEAP_MAGIC_FREE;
    pBlk->pNext = g_pFree;
    g_pFree = pBlk;
}

size_t
kheap_used_bytes(void)
{
    return g_cbUsed;
}
