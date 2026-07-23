/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Physical page freelist PMM (P-MEM-3) — dual low/high + hierarchical free.
 *
 * Greppable serial markers (implementation in kernel/mm/pmm.c):
 *   pmm: freelist free=
 *   pmm: high released free=
 *   pmm: orders tag=
 *   pmm: tib_design soft
 *   pmm: tib_host soft PASS | tib_host soft SKIP
 *   pmm: high_order soft
 *   pmm: soak PASS
 *   pmm: soak_tib PASS | soak_tib SKIP soft | soak_tib FAIL
 * Large-RAM soak: pmm_soak_tib(768ull<<30) from main (GJ_MEM=768G).
 * 1 TiB design observability is soft — no 1 TiB host required.
 */
#pragma once

#include <gj/types.h>

struct gj_mem_region {
    gj_paddr_t paBase;
    u64        cbLength; /* bytes */
};

void       pmm_init(const struct gj_mem_region *pRegions, size_t cRegions,
                    gj_paddr_t paKernelStart, gj_paddr_t paKernelEnd);
/**
 * After HHDM is mapped (P-MEM-5), release frames with PA ≥ 4 GiB into the
 * hierarchical freelist via free_range. Safe to call once; no-op if no
 * pending high regions or HHDM not ready.
 */
void       pmm_release_high(void);
gj_paddr_t pmm_alloc(void);
/**
 * Allocate cPages contiguous frames; 0 on failure.
 * Power-of-two counts ≤ 512 use hierarchical order freelists (+ split).
 */
gj_paddr_t pmm_alloc_pages(u32 cPages);
void       pmm_free(gj_paddr_t paPage);
/**
 * Free cPages starting at paPage. Exact power-of-two + natural alignment
 * → single hierarchical free node; else order-0 singles (frames never lost).
 */
void       pmm_free_pages(gj_paddr_t paPage, u32 cPages);
size_t     pmm_free_count(void);
u64        pmm_max_phys(void);
u64        pmm_total_frames(void);
/** Prefer high freelist when HHDM ready (desktop GPU/large allocs). */
gj_paddr_t pmm_alloc_high(void);
/**
 * Soak: alloc/free n singles + one contiguous block; return 0 on success.
 * Prints greppable `pmm: soak PASS` on success (smoke-all hard gate).
 */
int        pmm_soak(u32 u32Singles, u32 u32Contig);
/**
 * Hierarchical freelist node count for order ≥ 1; order 0 → free frame total.
 * Greppable diagnostics for hierarchical free.
 */
u64        pmm_order_count(u32 u32Order);
/**
 * True freelist *node* count for order (0..max). Unlike pmm_order_count(0),
 * order 0 here is single-page nodes only (not free-frame total).
 */
u64        pmm_order_nodes(u32 u32Order);
/** Free frames currently on low-zone freelists (PA &lt; 4 GiB). */
u64        pmm_free_frames_low(void);
/** Free frames currently on high-zone freelists (PA ≥ 4 GiB). */
u64        pmm_free_frames_high(void);
/** Sum of free nodes on orders 1..max (high-order soft observability). */
u64        pmm_high_order_nodes(void);
/**
 * Print greppable order histogram + tib_design / high_order soft markers.
 * Safe on any RAM size (1 TiB design observability; no host size required).
 */
void       pmm_log_orders(void);
/**
 * Large-RAM hierarchical soak (768 GiB class from main; optional 1 TiB).
 * If max phys &lt; u64NeedBytes, prints `pmm: soak_tib SKIP soft`, still runs
 * soft hierarchical exercise + order counts, and returns 0 (not a fail).
 * Success prints `pmm: soak_tib PASS`.
 */
int        pmm_soak_tib(u64 u64NeedBytes);
