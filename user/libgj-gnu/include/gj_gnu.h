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

/** Defined data export — GLOB_DAT / JUMP_SLOT resolve target (value 0x43). */
extern volatile uint64_t gj_gnu_export;

/** Optional init: restores gj_gnu_export = 0x43. */
void gj_gnu_init(void);

/** Canonical marker value for smokes / resolve checks. */
#define GJ_GNU_EXPORT_VALUE  ((uint64_t)0x43)

/*
 * Optional batch scaffold (src/graph_gnu_batch1.c) — not linked into the
 * product SO today. Declarations kept for future wire-up only.
 */
extern volatile uint64_t gj_gnu_batch1_export;
void gj_gnu_batch1_init(void);
uint64_t gj_gnu_batch1_id(void);

#define GJ_GNU_BATCH1_EXPORT_VALUE  ((uint64_t)0x471)

#ifdef __cplusplus
}
#endif
