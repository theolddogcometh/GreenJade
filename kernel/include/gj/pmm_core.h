/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Portable order-0 physical freelist core (shared C kernel).
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/shared/pmm_freelist.c (linked by aarch64 product).
 * x86_64 product keeps hierarchical pmm.c + gj/pmm.h; this core is the
 * minimal single-page freelist used where full dual-zone hierarchy is not
 * wired yet.
 *
 * Scope
 * -----
 * One contiguous identity-mapped physical pool [base, limit), LIFO free
 * list of 4 KiB frames. No zones, no hierarchical orders, no high-RAM
 * deferral. Enough for aarch64 bring-up, shared smokes, and soft selftest.
 *
 * Design anchors
 * --------------
 *   docs/X86_64_INTEL_PLATFORM.md  P-MEM-3 base unit still 4 KiB
 *   docs/GREENJADE_KERNEL_SPEC.md  early freelist; abandon phys==virt later
 *   STYLE.md                       Hungarian params; pure C freestanding
 *
 * Contract
 * --------
 *   Page size     = GJ_PMM_CORE_PAGE_SIZE (1 << 12), matches GJ_PAGE_SIZE
 *   Alloc return  = physical address, 0 if empty
 *   Free          = page-aligned PA previously allocated (or in pool)
 *   Not SMP-safe  unless caller serializes (single-CPU / locked path)
 *
 * Soft product surface
 * --------------------
 *   PMM_CORE_INIT      — build freelist over [base, limit)
 *   PMM_CORE_ALLOC     — one page; 0 empty
 *   PMM_CORE_FREE      — one page back to LIFO head
 *   PMM_CORE_SELFTEST  — OOB/unaligned/LIFO/payload soft PASS|FAIL
 *
 * Layering vs pmm.h
 * -----------------
 *   pmm_core  — portable order-0 only; shared object
 *   pmm       — x86 hierarchical + low/high + soak (product bar ≥ 1 TiB)
 * Callers that need contiguous multi-page or high-zone prefer pmm.h when
 * available; aarch64 scaffold uses this core until hierarchy is shared.
 *
 * greppable: PMM_CORE_INIT PMM_CORE_ALLOC PMM_CORE_FREE PMM_CORE_SELFTEST
 * greppable: GJ_PMM_CORE_PAGE_SIZE
 */
#pragma once

#include <gj/types.h>

/** Same 4 KiB geometry as GJ_PAGE_SIZE / GJ_PAGE_SHIFT (config). */
#define GJ_PMM_CORE_PAGE_SHIFT 12u
#define GJ_PMM_CORE_PAGE_SIZE  (1ul << GJ_PMM_CORE_PAGE_SHIFT)

/**
 * Init freelist over [u64Base, u64Limit) — both page-aligned physical
 * addresses that are identity-mapped for early bring-up.
 *
 * Frames are pushed free in walk order; subsequent alloc is LIFO from head.
 * Call once; re-init without draining live allocs is undefined.
 */
void gj_pmm_core_init(u64 u64Base, u64 u64Limit);

/**
 * Allocate one page from the freelist head.
 * Returns physical address, or 0 if the freelist is empty.
 * Returned PA is always page-aligned when non-zero.
 */
u64 gj_pmm_core_alloc(void);

/**
 * Free one page (must be in pool, page-aligned).
 * Soft selftest path rejects OOB/unaligned; product free assumes valid PA.
 * LIFO: next alloc may return this PA first.
 */
void gj_pmm_core_free(u64 u64Pa);

/** Frames currently on the freelist. */
unsigned gj_pmm_core_free_count(void);

/** Frames in the pool at init (free + outstanding allocs). */
unsigned gj_pmm_core_total_count(void);

/**
 * Soft self-check deepen: reject OOB/unaligned free, alloc align+range,
 * free-count steps, LIFO reuse, payload past node survives, pool restore.
 *
 * Returns 1 on PASS, 0 on soft FAIL.
 * Does not hard-gate boot; continuum / aarch64 smoke may assert the result.
 * greppable: PMM_CORE_SELFTEST
 */
int gj_pmm_core_selftest(void);
