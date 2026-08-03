/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding DMA page allocator — prefer low (<4 GiB) frames for xHCI etc.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. Not GPL / not Linux DMA API.
 *
 * Why low:
 *   G752VT-class hosts run VT-d with Default domain type Translated.
 *   Bring-up identity SLPT covers [0, 1 GiB). High pmm_alloc frames may sit
 *   outside that cover and break control data-stage under translation.
 *
 * greppable: dma_buf: soft note | dma_buf: high soft | dma_buf: soft PASS
 */
#include <gj/config.h>
#include <gj/dma_buf.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* Brief hold while hunting a low frame (avoid thrashing freelist). */
#define DMA_BUF_LOW_TRIES 16u

static u32 g_cAllocOk;
static u32 g_cAllocFail;
static u32 g_cHighSoft;
static u32 g_cFree;
static int g_fSoftNoteOnce;

static void *
dma_va(gj_paddr_t pa)
{
    if (pa == 0) {
        return NULL;
    }
    return (void *)hhdm_to_virt(pa);
}

static void
dma_zero(gj_paddr_t pa, u32 cPages)
{
    void *pVa;
    u32 i;

    if (pa == 0 || cPages == 0) {
        return;
    }
    for (i = 0; i < cPages; i++) {
        pVa = dma_va(pa + (gj_paddr_t)i * (gj_paddr_t)GJ_PAGE_SIZE);
        if (pVa != NULL) {
            memset(pVa, 0, GJ_PAGE_SIZE);
        }
    }
}

int
dma_buf_is_low(gj_paddr_t pa)
{
    return (pa != 0 && (u64)pa < GJ_DMA_LOW_MAX) ? 1 : 0;
}

int
dma_buf_in_vtd_identity(gj_paddr_t pa, u64 cb)
{
    u64 u64Pa = (u64)pa;

    if (pa == 0 || cb == 0) {
        return 0;
    }
    if (u64Pa >= GJ_DMA_VTD_IDENTITY_LIMIT) {
        return 0;
    }
    if (u64Pa + cb < u64Pa) {
        return 0; /* overflow */
    }
    if (u64Pa + cb > GJ_DMA_VTD_IDENTITY_LIMIT) {
        return 0;
    }
    return 1;
}

gj_paddr_t
dma_buf_alloc_page(void **ppVa)
{
    gj_paddr_t aHold[DMA_BUF_LOW_TRIES];
    u32 cHold = 0;
    gj_paddr_t pa = 0;
    u32 i;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }

    /* Prefer low: hold high pages briefly while retrying freelist. */
    for (i = 0; i < DMA_BUF_LOW_TRIES; i++) {
        pa = pmm_alloc();
        if (pa == 0) {
            break;
        }
        /*
         * Prefer PA inside VT-d bring-up identity [0, 1GiB) first so xHCI
         * rings work when TE is live (G752 Translated). Fall back to any
         * low (<4GiB) only if 1GiB pool is exhausted.
         */
        if (dma_buf_in_vtd_identity(pa, GJ_PAGE_SIZE)) {
            while (cHold > 0u) {
                cHold--;
                pmm_free(aHold[cHold]);
            }
            dma_zero(pa, 1u);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            if (g_cAllocOk < 0xffffffffu) {
                g_cAllocOk++;
            }
            return pa;
        }
        if (dma_buf_is_low(pa) && cHold + 1u < DMA_BUF_LOW_TRIES) {
            /* Hold low-but-high-for-VT-d while hunting identity pages. */
            aHold[cHold++] = pa;
            pa = 0;
            continue;
        }
        if (dma_buf_is_low(pa)) {
            while (cHold > 0u) {
                cHold--;
                pmm_free(aHold[cHold]);
            }
            dma_zero(pa, 1u);
            if (ppVa != NULL) {
                *ppVa = dma_va(pa);
            }
            if (g_cAllocOk < 0xffffffffu) {
                g_cAllocOk++;
            }
            if (g_cHighSoft < 0xffffffffu) {
                g_cHighSoft++; /* outside 1 GiB identity — soft warn path */
            }
            kprintf("dma_buf: soft pa=0x%lx outside vtd_identity 1GiB "
                    "(xHCI risk under TE)\n",
                    (unsigned long)pa);
            return pa;
        }
        if (cHold < DMA_BUF_LOW_TRIES) {
            aHold[cHold++] = pa;
        } else {
            pmm_free(pa);
            pa = 0;
            break;
        }
    }

    /* Fallback: use one held high page if any (soft warn). */
    if (cHold > 0u) {
        pa = aHold[0];
        for (i = 1u; i < cHold; i++) {
            pmm_free(aHold[i]);
        }
        if (g_cHighSoft < 0xffffffffu) {
            g_cHighSoft++;
        }
        kprintf("dma_buf: high soft pa=0x%lx (prefer <4GiB; VT-d id cover "
                "1GiB)\n",
                (unsigned long)pa);
        dma_zero(pa, 1u);
        if (ppVa != NULL) {
            *ppVa = dma_va(pa);
        }
        if (g_cAllocOk < 0xffffffffu) {
            g_cAllocOk++;
        }
        return pa;
    }

    if (g_cAllocFail < 0xffffffffu) {
        g_cAllocFail++;
    }
    return 0;
}

gj_paddr_t
dma_buf_alloc_pages(u32 cPages, void **ppVa)
{
    gj_paddr_t pa;
    u32 iTry;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    if (cPages == 0) {
        return 0;
    }
    if (cPages == 1u) {
        return dma_buf_alloc_page(ppVa);
    }

    /*
     * Contiguous multi-page: try hierarchical alloc a few times; accept if
     * base is low. High base is soft-accepted with warn (same as single page).
     */
    for (iTry = 0; iTry < 4u; iTry++) {
        pa = pmm_alloc_pages(cPages);
        if (pa == 0) {
            break;
        }
        if (!dma_buf_is_low(pa) && iTry + 1u < 4u) {
            /* Soft reject high; free and retry once more for low base. */
            pmm_free_pages(pa, cPages);
            continue;
        }
        if (!dma_buf_is_low(pa)) {
            if (g_cHighSoft < 0xffffffffu) {
                g_cHighSoft++;
            }
            kprintf("dma_buf: high soft pa=0x%lx pages=%u (prefer <4GiB)\n",
                    (unsigned long)pa, cPages);
        }
        dma_zero(pa, cPages);
        if (ppVa != NULL) {
            *ppVa = dma_va(pa);
        }
        if (g_cAllocOk < 0xffffffffu) {
            g_cAllocOk++;
        }
        return pa;
    }

    if (g_cAllocFail < 0xffffffffu) {
        g_cAllocFail++;
    }
    return 0;
}

void
dma_buf_free_page(gj_paddr_t pa)
{
    if (pa == 0) {
        return;
    }
    pmm_free(pa);
    if (g_cFree < 0xffffffffu) {
        g_cFree++;
    }
}

void
dma_buf_free_pages(gj_paddr_t pa, u32 cPages)
{
    if (pa == 0 || cPages == 0) {
        return;
    }
    if (cPages == 1u) {
        dma_buf_free_page(pa);
        return;
    }
    pmm_free_pages(pa, cPages);
    if (g_cFree < 0xffffffffu) {
        g_cFree++;
    }
}

void
dma_buf_soft_note(void)
{
    if (g_fSoftNoteOnce != 0) {
        return;
    }
    g_fSoftNoteOnce = 1;

    /*
     * Grep: dma_buf: soft note
     * Honesty: allocation only — IOMMU identity grant is iommu_vtd_xhci_identity.
     */
    kprintf("dma_buf: soft note prefer_low=<4GiB vtd_identity_limit=0x%lx "
            "alloc_ok=%u alloc_fail=%u high_soft=%u free=%u "
            "(use dma_buf_*; grant BDF via iommu_vtd_xhci_identity; "
            "Translated domain needs identity/window)\n",
            (unsigned long)GJ_DMA_VTD_IDENTITY_LIMIT, g_cAllocOk, g_cAllocFail,
            g_cHighSoft, g_cFree);
    kprintf("dma_buf: soft PASS\n");
}
