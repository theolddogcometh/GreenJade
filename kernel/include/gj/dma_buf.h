/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding DMA page allocator for device drivers (xHCI, virtio, etc.).
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0. Not GPL.
 *
 * Prefer low physical frames (PA < 4 GiB) so:
 *   - 32-bit / AC64=0 masters can program addresses without dual-cycle
 *   - VT-d bring-up identity SLPT covers [0, 1 GiB) (see gj/iommu.h)
 *
 * IOMMU identity requirements (soft note)
 * ---------------------------------------
 * Linux report on G752VT-class hosts: IOMMU default domain type Translated.
 * Under VT-d translation, freestanding drivers that hand raw pmm_alloc()
 * physical addresses as bus addresses may fail control data-stage DMA unless:
 *   1) buffers sit inside the identity cover (bring-up: first 1 GiB), and/or
 *   2) software windows + domain attach grant the BDF (iommu_vtd_xhci_identity)
 *   3) TE is soft-armed or HW-programmed when a DRHD is present
 *
 * This unit only allocates / frees / notes. Parent wires xHCI to use it and
 * calls iommu_vtd_xhci_identity(bus, slot, func) after iommu_probe().
 *
 * Greppable serial markers (kernel/mm/dma_buf.c)
 * ---------------------------------------------
 *   dma_buf: alloc page=… low=…   (optional)
 *   dma_buf: high soft pa=…       (fell back above 4 GiB)
 *   dma_buf: soft note …          (identity / low preference once)
 *   dma_buf: soft PASS
 *
 * greppable: DMA_BUF DMA_LOW VT-d identity
 */
#pragma once

#include <gj/types.h>

/** Preferred DMA ceiling: PA must be < this for "low" (4 GiB). */
#define GJ_DMA_LOW_MAX (0x100000000ull)

/**
 * Bring-up VT-d identity cover end (1 GiB). Matches VTD_IDENTITY_LIMIT in
 * iommu_vtd.c. Documented here so drivers need not include VT-d internals.
 * Soft requirement: keep DMA pages below this when TE is live and only the
 * identity SLPT is installed.
 */
#define GJ_DMA_VTD_IDENTITY_LIMIT (1024ull * 1024ull * 1024ull)

/**
 * Allocate one zeroed 4 KiB DMA page. Prefers PA < 4 GiB (retries briefly).
 * On success, *ppVa receives a kernel VA (HHDM or identity) when ppVa != NULL.
 * Returns physical address, or 0 on failure.
 */
gj_paddr_t dma_buf_alloc_page(void **ppVa);

/**
 * Allocate cPages contiguous zeroed frames (power-of-two preferred via PMM).
 * Prefers a base PA < 4 GiB. *ppVa is VA of the first page when non-NULL.
 * Returns base physical address, or 0 on failure.
 */
gj_paddr_t dma_buf_alloc_pages(u32 cPages, void **ppVa);

/** Free one page previously returned by dma_buf_alloc_page. */
void dma_buf_free_page(gj_paddr_t pa);

/** Free cPages starting at pa (from dma_buf_alloc_pages). */
void dma_buf_free_pages(gj_paddr_t pa, u32 cPages);

/** Non-zero if pa is in the preferred low DMA zone (PA < 4 GiB). */
int dma_buf_is_low(gj_paddr_t pa);

/**
 * Non-zero if [pa, pa+cb) lies inside the bring-up VT-d identity cover
 * [0, GJ_DMA_VTD_IDENTITY_LIMIT). Soft geometric check only — does not
 * query live IOMMU tables (use iommu_vtd_identity_covers for that).
 */
int dma_buf_in_vtd_identity(gj_paddr_t pa, u64 cb);

/**
 * Soft once note: low preference + VT-d identity requirements.
 * Never hard-gates. Logs greppable `dma_buf: soft note` / `dma_buf: soft PASS`.
 */
void dma_buf_soft_note(void);
