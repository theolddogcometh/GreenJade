/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Portable order-0 physical freelist core (shared C kernel).
 * Used by aarch64 product; x86 keeps hierarchical pmm.c.
 */
#pragma once

#include <gj/types.h>

#define GJ_PMM_CORE_PAGE_SHIFT 12u
#define GJ_PMM_CORE_PAGE_SIZE  (1ul << GJ_PMM_CORE_PAGE_SHIFT)

/**
 * Init freelist over [u64Base, u64Limit) — both page-aligned physical
 * addresses that are identity-mapped for early bring-up.
 */
void gj_pmm_core_init(u64 u64Base, u64 u64Limit);

/** Alloc one page; 0 on empty. */
u64 gj_pmm_core_alloc(void);

/** Free one page (must be in pool, page-aligned). */
void gj_pmm_core_free(u64 u64Pa);

unsigned gj_pmm_core_free_count(void);
unsigned gj_pmm_core_total_count(void);

/** Soft self-check: alloc+free one page; returns 1 on ok. */
int gj_pmm_core_selftest(void);
