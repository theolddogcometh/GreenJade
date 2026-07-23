/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Public surface of product SO libgj-gnu.so.1 (DT_GNU_HASH).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Canonical marker value for smokes / resolve checks. */
#define GJ_GNU_EXPORT_VALUE         ((uint64_t)0x43)

/** Soft stamp value ('GNU1') — companion dynsym, not a smoke contract. */
#define GJ_GNU_SOFT_STAMP_VALUE     ((uint64_t)0x474E5531ull)

/** Soft capability bits present when soft deepen surface is linked. */
#define GJ_GNU_SOFT_CAP_GET         ((uint32_t)0x1u)
#define GJ_GNU_SOFT_CAP_ID          ((uint32_t)0x2u)
#define GJ_GNU_SOFT_CAP_PROBE       ((uint32_t)0x4u)
#define GJ_GNU_SOFT_CAP_TOUCH       ((uint32_t)0x8u)
#define GJ_GNU_SOFT_CAP_MASK        ((uint32_t)0xfu)

/** Defined data export — GLOB_DAT / JUMP_SLOT resolve target (value 0x43). */
extern volatile uint64_t gj_gnu_export;

/** Soft stamp data export — enriches .gnu.hash; not a smoke contract. */
extern volatile uint64_t gj_gnu_soft_stamp;

/** Soft capability word (bitmask of soft deepen surface). */
extern volatile uint32_t gj_gnu_soft_caps;

/** Optional init: restores gj_gnu_export / soft stamp / soft caps. */
void gj_gnu_init(void);

/** Soft get: current product export value (read-only). */
uint64_t gj_gnu_soft_get(void);

/** Soft id: current soft stamp (identity for multi-SO soft probes). */
uint64_t gj_gnu_soft_id(void);

/**
 * Soft probe: 1 if export is canonical; on soft miss restores carefully
 * and returns 0. Never hard-fails.
 */
int gj_gnu_soft_probe(void);

/**
 * Soft touch: re-assert export + stamp + caps; returns canonical export.
 */
uint64_t gj_gnu_soft_touch(void);

/*
 * Optional batch scaffold (src/graph_gnu_batch1.c) — not linked into the
 * product SO today. Declarations kept for future wire-up only.
 */
#define GJ_GNU_BATCH1_EXPORT_VALUE  ((uint64_t)0x471)
#define GJ_GNU_BATCH1_SOFT_STAMP    ((uint64_t)0x42317331ull) /* 'B1s1' */

extern volatile uint64_t gj_gnu_batch1_export;
extern volatile uint64_t gj_gnu_batch1_soft_stamp;
void gj_gnu_batch1_init(void);
uint64_t gj_gnu_batch1_id(void);
uint64_t gj_gnu_batch1_soft_get(void);
int gj_gnu_batch1_soft_probe(void);

#ifdef __cplusplus
}
#endif
