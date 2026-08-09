/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Physical page freelist PMM (P-MEM-3) - dual low/high + hierarchical free.
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/mm/pmm.c (x86_64 product hierarchical path).
 * Portable order-0 subset: gj/pmm_core.h (aarch64 shared C kernel).
 *
 * Scope
 * -----
 * Frame allocator for product RAM: 4 KiB base order, hierarchical free nodes
 * for power-of-two contiguous blocks, dual freelists split at 4 GiB so early
 * identity-only bring-up can free low RAM while high frames wait for HHDM.
 *
 * Design anchors
 * --------------
 *   docs/X86_64_INTEL_PLATFORM.md  P-MEM-3 (>=1 TiB bar), P-MEM-5 (HHDM unlock)
 *   docs/IMPLEMENTATION.md         freelist + 768 GiB soak gate
 *   docs/GREENJADE_KERNEL_SPEC.md  zone/region freelists; cover all RAM
 *   docs/SECURITY_CORE_DESIGN.md   DoS: unbounded alloc is in-scope
 *
 * Zone model
 * ----------
 *   Low  : PA < 4 GiB  - available after pmm_init (identity-mapped)
 *   High : PA >= 4 GiB  - parked until vmm_hhdm_init + pmm_release_high
 *
 * Hierarchical orders
 * -------------------
 * Order n holds free blocks of 2^n frames (order 0 = single 4 KiB).
 * pmm_alloc_pages power-of-two <= 512 splits from higher orders; free of
 * exact power-of-two + natural alignment coalesces into one node; else
 * frees as order-0 singles so frames are never lost.
 *
 * Soft product surface
 * --------------------
 *   PMM_INIT           - region walk; reserve kernel image; seed freelists
 *   PMM_RELEASE_HIGH   - after HHDM: free parked high ranges
 *   PMM_HIERARCHICAL   - order freelists + split/coalesce
 *   PMM_SOAK           - singles + contig smoke (hard gate)
 *   PMM_SOAK_TIB       - large-RAM hierarchical soak (soft SKIP if host small)
 *   PMM_ORDERS_LOG     - greppable histogram + tib_design / high_order soft
 *
 * Lean DMA residual (Soft!=product; UDX rings eng / force32)
 * ---------------------------------------------------------
 *   pmm_alloc / pmm_alloc_pages exhaust the *entire* low-zone hierarchy
 *   (exact order, then split higher low orders) before any high-zone
 *   (PA >= 4 GiB) pop. Keeps DMA-capable pages (PA < 4 GiB) available for
 *   rtl8168 TX/RX rings via dma_buf force32 / VT-d identity [0, 1 GiB) and
 *   UDX ring eng (rtl8168_udx / xhci_udx OPEN_UDX over dual MIT/Apache).
 *
 *   pmm_alloc_low / pmm_alloc_pages_low: low-zone *only* (never high
 *   fallback) - DMA page-alloc foundation for force32 / UDX rings when
 *   callers must not accept PA >= 4 GiB. Soft tallies only - never product
 *   close. G-AC-1: soft residual != product AC; dual MIT OR Apache-2.0.
 *
 *   Force32 residual deepen (Soft!=product; not product AC):
 *     - Low zone = PA < 4 GiB (this API contract; never high hand-out).
 *     - VT-d bring-up identity = [0, 1 GiB) subset of low (dma_buf /
 *       iommu force32 filter still required for TE-safe bus=PA).
 *     - pmm_alloc_low is the PMM foundation; force32 identity preference
 *       lives in dma_buf / UDX host eng - Soft residual != product close.
 *     Soft residual lamps: pmm: soft residual lean | force32 | alloc_low.
 *   No version stamp. Soft!=product. Soft residual != product dual-license
 *   driver path (userspace UDX).
 *
 * Layering
 * --------
 *   Boot hands gj_mem_region[] -> pmm_init
 *   vmm_hhdm_init -> pmm_release_high
 *   vmm / memobj / kheap / drivers call pmm_alloc* / pmm_free*
 *   dma_buf / UDX ring eng: pmm_alloc*_low or pmm_alloc* low-first
 *
 * Greppable serial markers (kernel/mm/pmm.c)
 * ------------------------------------------
 *   pmm: freelist free=
 *   pmm: high released free=
 *   pmm: orders tag=
 *   pmm: tib_design soft
 *   pmm: tib_host soft PASS | tib_host soft SKIP
 *   pmm: high_order soft
 *   pmm: soft residual lean
 *   pmm: soft residual force32
 *   pmm: soft residual alloc_low
 *   pmm: soft dma
 *   pmm: soak PASS
 *   pmm: soak_tib PASS | soak_tib SKIP soft | soak_tib FAIL
 *
 * Large-RAM soak: pmm_soak_tib(768ull<<30) from main (GJ_MEM=768G).
 * 1 TiB design observability is soft - no 1 TiB host required.
 *
 * greppable: PMM_INIT PMM_RELEASE_HIGH PMM_HIERARCHICAL P-MEM-3
 * greppable: PMM_SOAK PMM_SOAK_TIB PMM_ORDERS_LOG
 * greppable: pmm: soft residual lean | pmm: soft residual force32
 * greppable: pmm: soft residual alloc_low | pmm: soft dma | dma_low_first
 * greppable: pmm_alloc_low pmm_alloc_pages_low force32 UDX
 */
#pragma once

#include <gj/types.h>

/**
 * One contiguous usable RAM region from firmware/boot (bytes, not frames).
 * paBase page-aligned preferred; cbLength may be rounded down by init.
 */
struct gj_mem_region {
    gj_paddr_t paBase;
    u64        cbLength; /* bytes */
};

/**
 * Soft: reserve [paBase, paBase+cbLen) from freelist (UEFI soft media .ko).
 * Call before pmm_init when boot_info soft media fields are set. Soft!=product.
 * No-op if cbLen==0 or paBase==0. Grep: pmm: soft media reserve
 */
void pmm_soft_reserve(gj_paddr_t paBase, u64 cbLen);

/**
 * Build freelists from usable regions; exclude [paKernelStart, paKernelEnd)
 * and any prior pmm_soft_reserve span.
 *
 * Call once early, before product alloc. High (PA >= 4 GiB) ranges may be
 * deferred until pmm_release_high. Invalid/overlapping regions soft-skip.
 */
void pmm_init(const struct gj_mem_region *pRegions, size_t cRegions,
              gj_paddr_t paKernelStart, gj_paddr_t paKernelEnd);

/**
 * After HHDM is mapped (P-MEM-5), release frames with PA >= 4 GiB into the
 * hierarchical freelist via free_range.
 *
 * Safe to call once; no-op if no pending high regions or HHDM not ready
 * (hhdm_ready() == 0). Logs greppable `pmm: high released free=`.
 */
void pmm_release_high(void);

/**
 * Allocate one 4 KiB frame (order 0). Returns physical address, or 0 if empty.
 * Prefer low freelist unless product path uses pmm_alloc_high.
 */
gj_paddr_t pmm_alloc(void);

/**
 * Allocate cPages contiguous frames; 0 on failure.
 * Power-of-two counts <= 512 use hierarchical order freelists (+ split).
 * Non-power-of-two or larger: best-effort contiguous search / multi-alloc soft.
 */
gj_paddr_t pmm_alloc_pages(u32 cPages);

/**
 * DMA foundation: one 4 KiB frame from low-zone hierarchy only (PA < 4 GiB).
 * Exhausts exact order-0 then splits higher *low* orders; never hands high.
 * Returns 0 if low free is empty (even if high still has frames).
 *
 * Soft residual deepen (Soft!=product; dual MIT OR Apache-2.0):
 *   Force32 / UDX ring eng foundation - PA always < 4 GiB on success.
 *   VT-d identity force32-safe window is [0, 1 GiB) (subset of low); this
 *   API does not filter identity - callers (dma_buf force32 / UDX host)
 *   still prefer or require PA in identity under TE. Soft tallies
 *   api_alloc_low ok/fail; never product AC close. G-AC-1.
 * Grep: pmm_alloc_low | pmm: soft residual alloc_low | force32
 */
gj_paddr_t pmm_alloc_low(void);

/**
 * DMA foundation: contiguous low-zone pages only (PA < 4 GiB).
 * Power-of-two <= 512 via low hierarchical split; else low order-0 scan.
 * Never returns PA >= 4 GiB; 0 if low cannot satisfy. Soft!=product.
 *
 * Soft residual: multi-page force32 / UDX ring eng foundation (desc +
 * slot arrays). Same force32 identity note as pmm_alloc_low - identity
 * filter remains at dma_buf / UDX layer. Grep: pmm_alloc_pages_low
 */
gj_paddr_t pmm_alloc_pages_low(u32 cPages);

/**
 * Free one frame previously returned by pmm_alloc / single-page path.
 * paPage must be page-aligned and owned; double-free is undefined (fail soft).
 */
void pmm_free(gj_paddr_t paPage);

/**
 * Free cPages starting at paPage.
 * Exact power-of-two + natural alignment -> single hierarchical free node;
 * else order-0 singles (frames never lost).
 */
void pmm_free_pages(gj_paddr_t paPage, u32 cPages);

/** Approximate free frame count (all zones / orders, product observability). */
size_t pmm_free_count(void);

/**
 * Highest physical byte exclusive bound observed at init (for HHDM paMax).
 * 0 if PMM not initialized.
 */
u64 pmm_max_phys(void);

/** Total frames managed (free + allocated), after init. */
u64 pmm_total_frames(void);

/**
 * Prefer high freelist when HHDM ready (desktop GPU / large allocs).
 * Falls back to low if high empty. Returns 0 on total exhaustion.
 */
gj_paddr_t pmm_alloc_high(void);

/**
 * Soak: alloc/free n singles + one contiguous block; return 0 on success.
 * Prints greppable `pmm: soak PASS` on success (smoke-all hard gate).
 * Non-zero on failure (do not ignore in continuum / smoke).
 */
int pmm_soak(u32 u32Singles, u32 u32Contig);

/**
 * Hierarchical freelist node count for order >= 1; order 0 -> free frame total.
 * Greppable diagnostics for hierarchical free (not pure node count at 0).
 */
u64 pmm_order_count(u32 u32Order);

/**
 * True freelist *node* count for order (0..max). Unlike pmm_order_count(0),
 * order 0 here is single-page nodes only (not free-frame total).
 */
u64 pmm_order_nodes(u32 u32Order);

/** Free frames currently on low-zone freelists (PA < 4 GiB). */
u64 pmm_free_frames_low(void);

/** Free frames currently on high-zone freelists (PA >= 4 GiB). */
u64 pmm_free_frames_high(void);

/** Sum of free nodes on orders 1..max (high-order soft observability). */
u64 pmm_high_order_nodes(void);

/**
 * Print greppable order histogram + tib_design / high_order soft markers.
 * Safe on any RAM size (1 TiB design observability; no host size required).
 */
void pmm_log_orders(void);

/**
 * Large-RAM hierarchical soak (768 GiB class from main; optional 1 TiB).
 *
 * If max phys < u64NeedBytes, prints `pmm: soak_tib SKIP soft`, still runs
 * soft hierarchical exercise + order counts, and returns 0 (not a fail).
 * Success prints `pmm: soak_tib PASS`. Hard FAIL only when host is large
 * enough and hierarchical exercise breaks invariants.
 */
int pmm_soak_tib(u64 u64NeedBytes);
