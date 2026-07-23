/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Public surface of product SO libgj-so.so.1 (SysV DT_HASH).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Defined data export — GLOB_DAT / JUMP_SLOT resolve target (value 0x42). */
extern volatile uint64_t gj_so_export;

/** Optional init: restores gj_so_export = 0x42. */
void gj_so_init(void);

/** Function export: returns current gj_so_export (JUMP_SLOT-friendly). */
uint64_t gj_so_get_export(void);

/**
 * Classic System V ELF name hash (DT_HASH). Same algorithm as the
 * kernel / ld-gj SysV walkers. NULL name → 0.
 */
uint32_t gj_so_sysv_hash(const char *szName);

/** Canonical marker value for smokes / resolve checks. */
#define GJ_SO_EXPORT_VALUE  ((uint64_t)0x42)

/*
 * Optional soft batch scaffolds (src/graph_so_batch*.c) — not linked into
 * the product SO today. Declarations kept for future wire-up only.
 */

/* --- batch 1: SysV hash helpers + presence marker --------------------- */
extern volatile uint64_t gj_so_batch1_export;
void gj_so_batch1_init(void);
uint64_t gj_so_batch1_id(void);
uint32_t gj_so_batch1_sysv_hash(const char *szName);
int gj_so_batch1_streq(const char *szA, const char *szB);
uint32_t gj_so_batch1_bucket(uint32_t u32Hash, uint32_t u32Nbucket);

#define GJ_SO_BATCH1_EXPORT_VALUE  ((uint64_t)0x421)

/* --- batch 2: multi-marker + soft chain walk -------------------------- */
extern volatile uint64_t gj_so_batch2_export_a;
extern volatile uint64_t gj_so_batch2_export_b;
void gj_so_batch2_init(void);
uint64_t gj_so_batch2_id(void);
uint32_t gj_so_batch2_chain_next(const uint32_t *pChain, uint32_t u32Idx,
    uint32_t u32Nchain);
uint32_t gj_so_batch2_chain_find(const uint32_t *pChain, uint32_t u32Nchain,
    uint32_t u32Start, int (*pfnMatch)(uint32_t u32Idx, void *pCtx),
    void *pCtx);

#define GJ_SO_BATCH2_EXPORT_A_VALUE  ((uint64_t)0x422)
#define GJ_SO_BATCH2_EXPORT_B_VALUE  ((uint64_t)0x423)

#ifdef __cplusplus
}
#endif
