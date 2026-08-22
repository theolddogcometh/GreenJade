/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Lean freestanding DMA residual for UDX NIC/USB DMA caps eng:
 * NIC rings (force32 + map + sync) + xHCI-class USB HC rings + residual
 * UDX/DDI-shaped helpers (coherent / map_single / window_ok / DDI ops).
 * Pure C11. Dual license: MIT OR Apache-2.0. Not GPL. Soft!=product.
 *
 * Prefer low physical frames (PA < 4 GiB) so:
 *   - 32-bit / AC64=0 masters can program addresses without dual-cycle
 *   - VT-d bring-up identity SLPT covers [0, 1 GiB) (see gj/iommu.h)
 *
 * Freestanding NIC rings (rtl8168-class, addr_hi=0)
 * ------------------------------------------------
 * Desc page + per-slot TX/RX buf pages must sit in VT-d identity under TE
 * or OWN never clears (R0/R1 killer). Use:
 *   dma_buf_alloc_ring_page / free_ring_page   - one force32 ring page
 *   dma_buf_alloc_ring_slots / free_ring_slots - all-or-nothing ring bufs
 *   dma_buf_alloc_page_force32                 - strict identity single
 *   dma_buf_map(..., fForce32=1)               - bus=PA or FAIL
 *   dma_buf_sync_*                             - clflush under re-post
 *     Policy (PURE_C_CONCURRENCY / HALL_OF_SHAME Soft!=product):
 *     narrow dirty lines + one mfence at publish (dma_clflush_range);
 *     no wbinvd; no clflush spam per dword; no mfence-after-every-store.
 *
 * DDI DMA_BUF residual (UDX host foundation; deepen alloc/free/map)
 * ----------------------------------------------------------------
 * ddi_door ops DMA_BUF_ALLOC|FREE|MAP (ops 10/11/12) call:
 *   ALLOC -> dma_buf_alloc_page[_force32] / alloc_pages[_force32]
 *   FREE  -> dma_buf_unmap (soft) + dma_buf_free_page / free_pages
 *   MAP   -> dma_buf_window_ok (force32) + dma_buf_map + mapping_error
 * Soft map-live bookkeeping tracks residual maps (not product IOMMU).
 * Soft pages-live tracks outstanding alloc residual (not product quota).
 * Soft DDI alloc-live tracks ddi_alloc residual (not product handle quota):
 *   ddi_alloc records slot + window_ok honesty; ddi_free unmaps+drops;
 *   ddi_map idempotent same-cb rematch / remap different cb + sync_device.
 * Soft free residual: free_page/free_pages drop map+DDI live in range.
 * Soft free-miss: ddi_free without prior soft ddi_alloc still frees (capped).
 * Soft mint honesty: never mints DMA window / CNode caps here
 *   (mint=OPEN; pair DDI DMA_NOTE / iommu_window_grant / product path).
 * DDI-shaped wrappers: dma_buf_ddi_alloc / ddi_free / ddi_map (lean deepen).
 * Soft!=product | G-AC-1 (not Linux DMA API complete; not .ko product).
 *
 * IOMMU identity (soft note)
 * --------------------------
 * G752VT-class: IOMMU default domain Translated. Freestanding drivers that
 * hand raw pmm_alloc() PAs as bus addresses may fail under TE unless:
 *   1) buffers sit inside the identity cover (bring-up: first 1 GiB), and/or
 *   2) software windows + domain attach grant the BDF (iommu_vtd_*)
 *   3) TE is soft-armed or HW-programmed when a DRHD is present
 *
 * This unit allocates / frees / maps / syncs. Soft!=product: no VT-d window
 * grant here (pair iommu_vtd_* / DDI DMA_NOTE). Parent wires freestanding
 * NIC/xHCI; IOMMU attach + product DMA window caps are owned elsewhere.
 * Continuous RX re-post under arping uses dma_buf_sync_* (clflush) so
 * OWN/desc lines stay coherent with the NIC.
 *
 * UDX/DDI mapping residual
 * ------------------------
 *   dma_buf_alloc_coherent / free_coherent  - size-based zeroed pages
 *   dma_buf_map_single / unmap_single       - HHDM VA -> identity bus=PA
 *   dma_buf_window_ok                       - geometric precheck for UDX caps
 *   dma_buf_ddi_alloc / ddi_free / ddi_map  - DDI DMA_BUF op residual
 * Soft!=product: no live DMA window cap mint (DDI/devmgr/iommu own grants).
 * map_single uses hhdm_to_phys (kernel/HHDM VA only - not arbitrary user VA).
 *
 * Dual DoD B residual: stable DMA for freestanding NIC rings under arping.
 * USB residual: same force32 identity path for xHCI ring/TRB pages under TE.
 * soft PASS != DUT R-climb / interactive SSH login; soft PASS != product UDX DMA caps.
 * Fail/soft-warn logs hard-capped so continuous re-post does not flood serial.
 * No stamp storms. No version stamp. Lean residual only (G-AC-1).
 *
 * Greppable serial markers (kernel/mm/dma_buf.c)
 * ---------------------------------------------
 *   dma_buf: high soft pa=...       (fell back above 4 GiB; Soft!=product)
 *   dma_buf: soft pa=... force32=1 outside vtd_identity
 *   dma_buf: force32 FAIL         (strict identity path; Soft!=product)
 *   dma_buf: soft ring FAIL|ok    (ring page/slots; fail capped)
 *   dma_buf: soft map FAIL|ok     (map honesty; fail capped)
 *   dma_buf: soft map_single FAIL (VA->bus; fail capped)
 *   dma_buf: soft coherent FAIL   (size alloc; fail capped)
 *   dma_buf: soft window FAIL     (UDX caps geometry; fail capped)
 *   dma_buf: soft sync FAIL       (sync honesty; fail capped)
 *   dma_buf: soft free mapped     (free while soft-map live; capped)
 *   dma_buf: soft unmap miss      (unmap without soft-map; capped)
 *   dma_buf: soft free miss       (ddi_free without soft ddi live; capped)
 *   dma_buf: soft ddi ...         (DDI DMA_BUF residual; mint honesty)
 *   dma_buf: soft ddi deepen      (alloc/free/map residual deepen once)
 *   dma_buf: soft mint honesty    (mint=OPEN; Soft!=product once)
 *   dma_buf: soft residual ...    (Dual DoD B + NIC/USB + UDX/DDI once)
 *   dma_buf: soft note ...        (counters once)
 *   dma_buf: soft PASS
 *
 * greppable: DMA_BUF DMA_LOW VT-d identity force32 dma_buf: soft ring
 * greppable: Soft!=product (serial) | Soft!=product (comments)
 * Soft != product | G-AC-1 (not Linux DMA API complete)
 * DMA buf for UDX caps | UDX NIC/USB DMA caps eng | freestanding residual lean
 * greppable: xHCI USB residual lean | Dual DoD B NIC rings
 * greppable: DDI DMA_BUF residual | soft mint honesty | soft map live
 * greppable: soft ddi live | soft ddi deepen | soft free miss
 */
#pragma once

#include <gj/types.h>

/** Preferred DMA ceiling: PA must be < this for "low" (4 GiB). */
#define GJ_DMA_LOW_MAX (0x100000000ull)

/**
 * Bring-up VT-d identity cover end (1 GiB). Matches VTD_IDENTITY_LIMIT in
 * iommu_vtd.c. Documented here so drivers need not include VT-d internals.
 * Soft requirement: keep DMA pages below this when TE is live and only the
 * identity SLPT is installed.
 */
#define GJ_DMA_VTD_IDENTITY_LIMIT (1024ull * 1024ull * 1024ull)

/**
 * Soft DMA direction (Linux-shaped ints). Soft!=product full enum/ABI.
 * Used by map/sync only; freestanding identity path still returns PA as bus.
 * Aligns with UDX_DMA_* ordinals for residual mapping story.
 */
#define DMA_BUF_BIDIRECTIONAL 0
#define DMA_BUF_TO_DEVICE     1
#define DMA_BUF_FROM_DEVICE   2

/**
 * Allocate one zeroed 4 KiB DMA page.
 * Preference: VT-d identity [0, 1 GiB) first (force32-safe under TE), then
 * any PA < 4 GiB, then high (soft warn). *ppVa is kernel VA when non-NULL.
 * Returns physical address, or 0 on failure.
 * Soft residual: pages-live++ (Soft!=product; not product quota).
 */
gj_paddr_t dma_buf_alloc_page(void **ppVa);

/**
 * Strict force32 + VT-d identity page: only returns PA in [0, 1 GiB).
 * Prefer this for freestanding NIC/xHCI rings under TE (OWN-safe).
 * Returns 0 if no identity page is available (does not hand high PA).
 * Grep: dma_buf: force32
 */
gj_paddr_t dma_buf_alloc_page_force32(void **ppVa);

/**
 * Allocate cPages contiguous zeroed frames (power-of-two preferred via PMM).
 * Prefers base PA in VT-d identity [0, 1 GiB), then < 4 GiB.
 * *ppVa is VA of the first page when non-NULL.
 * Returns base physical address, or 0 on failure.
 */
gj_paddr_t dma_buf_alloc_pages(u32 cPages, void **ppVa);

/**
 * Strict force32 multi-page: only returns base PA where [pa, pa+cPages*4K)
 * sits entirely in VT-d identity [0, 1 GiB). Returns 0 rather than high PA.
 * Grep: dma_buf: force32
 */
gj_paddr_t dma_buf_alloc_pages_force32(u32 cPages, void **ppVa);

/**
 * Freestanding NIC/USB ring page: force32 identity alloc + map (bus=PA) +
 * sync_for_device so zeroed lines are DRAM-visible before OWN/cycle program.
 * One 4 KiB page. Returns bus cookie (=PA) or 0. Soft!=product.
 * Grep: dma_buf: soft ring
 */
gj_paddr_t dma_buf_alloc_ring_page(void **ppVa);

/** Free one page from dma_buf_alloc_ring_page (unmap + free). */
void dma_buf_free_ring_page(gj_paddr_t pa);

/**
 * All-or-nothing freestanding NIC ring slots (per-buf pages).
 * Fills paOut[0..cSlots) and optional ppVaOut[i]. On any fail: frees
 * partial, zeros outputs, returns -1. Success returns 0.
 * Soft!=product. Grep: dma_buf: soft ring
 */
int dma_buf_alloc_ring_slots(u32 cSlots, gj_paddr_t *paOut, void **ppVaOut);

/** Free cSlots pages previously filled by dma_buf_alloc_ring_slots. */
void dma_buf_free_ring_slots(u32 cSlots, gj_paddr_t *paIn);

/**
 * UDX/DDI-shaped coherent alloc: round cb up to whole pages, zero, return
 * base PA (identity bus). *ppVa is HHDM VA of first page when non-NULL.
 * fForce32 non-zero: identity cover required (OWN-safe under TE).
 * Soft!=product: no IOMMU grant - pair DDI DMA_NOTE / iommu_window_grant.
 * Grep: dma_buf: soft coherent
 */
gj_paddr_t dma_buf_alloc_coherent(u64 cb, void **ppVa, int fForce32);

/** Free buffer from dma_buf_alloc_coherent (cb used only to size pages). */
void dma_buf_free_coherent(gj_paddr_t pa, u64 cb);

/**
 * Free one page previously returned by dma_buf_alloc_page.
 * Soft residual free deepen: drops soft-map-live + soft DDI live covering
 * this page, then pmm_free.
 * Soft!=product: not product IOMMU revoke / DMA window cap destroy.
 */
void dma_buf_free_page(gj_paddr_t pa);

/**
 * Free cPages starting at pa (from dma_buf_alloc_pages).
 * Soft residual free deepen: soft-unmap + DDI live drop in range, then
 * pmm_free_pages.
 */
void dma_buf_free_pages(gj_paddr_t pa, u32 cPages);

/** Non-zero if pa is in the preferred low DMA zone (PA < 4 GiB). */
int dma_buf_is_low(gj_paddr_t pa);

/**
 * Non-zero if [pa, pa+cb) lies inside the bring-up VT-d identity cover
 * [0, GJ_DMA_VTD_IDENTITY_LIMIT). Soft geometric check only - does not
 * query live IOMMU tables (use iommu_vtd_identity_covers for that).
 */
int dma_buf_in_vtd_identity(gj_paddr_t pa, u64 cb);

/**
 * Soft geometric ok for a DMA window grant note (UDX caps / DDI residual).
 * fForce32 non-zero: range must sit in VT-d identity. Non-force32: non-zero
 * non-overflowing range only (caller risk under TE outside identity).
 * Soft!=product: does not mint caps or call iommu_window_grant.
 * Grep: dma_buf: soft window
 */
int dma_buf_window_ok(gj_paddr_t pa, u64 cb, int fForce32);

/**
 * Map [pa, pa+cb) for device DMA.
 * Soft identity bus address = pa when cover ok. Returns 0 on fail.
 * fForce32 non-zero: FAIL unless range sits in VT-d identity [0, 1 GiB)
 * (OWN would stick under TE for force32 masters like rtl8168).
 * Soft residual: records soft map-live slot (not product IOMMU PTE).
 * Soft mint honesty: never mints DMA window / CNode caps (mint=OPEN).
 * Soft!=product: no IOMMU window grant (pair iommu_vtd_*). nDir is soft.
 * Grep: dma_buf: soft map | soft mint honesty
 */
gj_paddr_t dma_buf_map(gj_paddr_t pa, u64 cb, int nDir, int fForce32);

/**
 * UDX-shaped map_single: HHDM/kernel VA -> identity bus (PA).
 * Soft: hhdm_to_phys then dma_buf_map. Not for arbitrary user VA
 * (product AS path uses DDI grants + process virt_to_phys).
 * Grep: dma_buf: soft map_single
 */
gj_paddr_t dma_buf_map_single(void *pVa, u64 cb, int nDir, int fForce32);

/**
 * Unmap a prior dma_buf_map cookie. Soft bookkeeping only - no IOMMU revoke.
 * Soft residual: clears soft map-live slot when present.
 * Soft!=product: not product window/cap destroy (mint=OPEN remains).
 * Grep: dma_buf: soft map (unmap counters in soft note)
 */
void dma_buf_unmap(gj_paddr_t paDma, u64 cb, int nDir);

/** UDX-shaped unmap_single alias (same soft bookkeeping as dma_buf_unmap). */
void dma_buf_unmap_single(gj_paddr_t paDma, u64 cb, int nDir);

/** Non-zero if map cookie is a failure (zero bus address). */
int dma_buf_mapping_error(gj_paddr_t paDma);

/**
 * DDI DMA_BUF_ALLOC residual (UDX host foundation; deepen).
 * cPages 0 -> 1; fForce32 non-zero selects identity force32 path.
 * Soft bound: cPages > 16 fails (matches ddi_door soft residual bound).
 * Soft residual: records soft DDI alloc-live + window_ok honesty.
 * Soft!=product: not DMA window / CNode cap mint (mint=OPEN).
 * Grep: dma_buf: soft ddi | soft ddi deepen
 */
gj_paddr_t dma_buf_ddi_alloc(u32 cPages, int fForce32, void **ppVa);

/**
 * DDI DMA_BUF_FREE residual deepen: soft unmap live + free pages.
 * cPages 0 uses soft DDI alloc-live cPages when present (else 1).
 * Free-miss (no soft ddi live) still frees pages (capped log).
 * Soft!=product: not product IOMMU revoke.
 * Grep: dma_buf: soft ddi | soft free miss
 */
void dma_buf_ddi_free(gj_paddr_t pa, u32 cPages);

/**
 * DDI DMA_BUF_MAP residual deepen: window_ok when force32 + map (BIDIR).
 * Soft: inherit force32 from soft DDI alloc-live; idempotent same-cb;
 * rematch different cb soft-unmaps prior; sync_pa_for_device residual.
 * Soft mint honesty: identity bus=PA only; no window cap mint.
 * Grep: dma_buf: soft ddi | soft mint honesty | soft ddi deepen
 */
gj_paddr_t dma_buf_ddi_map(gj_paddr_t pa, u64 cb, int fForce32);

/**
 * Sync for CPU after device write (FROM_DEVICE / BIDIR): clflush range +
 * mfence so continuous RX re-post under arping sees NIC DRAM, not stale
 * cache (4 descs/line stick). Returns 0 ok, -1 fail (null/zero).
 * Soft!=product full CWB / IOMMU IOTLB flush. Grep: dma_buf: soft sync
 */
int dma_buf_sync_for_cpu(void *pVa, u64 cb, int nDir);

/**
 * Sync for device before DMA read (TO_DEVICE / BIDIR): clflush + mfence so
 * NIC sees OWN/opts/payload written by CPU. Returns 0 ok, -1 fail.
 * Soft!=product. Grep: dma_buf: soft sync
 */
int dma_buf_sync_for_device(void *pVa, u64 cb, int nDir);

/**
 * Convenience: sync by PA via HHDM VA (freestanding ring/buf pages).
 * Same return contract as dma_buf_sync_for_*. UDX residual: dma cookie = PA.
 */
int dma_buf_sync_pa_for_cpu(gj_paddr_t pa, u64 cb, int nDir);
int dma_buf_sync_pa_for_device(gj_paddr_t pa, u64 cb, int nDir);

/**
 * Soft once note: low preference + VT-d identity + map/sync + ring residual
 * + DDI DMA_BUF alloc/free/map residual deepen + soft mint honesty.
 * Never hard-gates. Lean one-shot (no stamp storms):
 *   dma_buf: soft note | dma_buf: soft residual | dma_buf: soft mint honesty
 *   | dma_buf: soft ddi residual | dma_buf: soft ddi deepen | dma_buf: soft PASS
 * Soft != product | Dual DoD B residual (soft PASS != interactive SSH login)
 * | freestanding NIC/USB rings | UDX NIC/USB DMA caps eng residual
 * | DDI DMA_BUF residual deepen (soft PASS != product UDX DMA caps; G-AC-1).
 */
void dma_buf_soft_note(void);
