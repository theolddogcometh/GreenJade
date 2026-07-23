/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Public surface of product SO libgj-so.so.1 (SysV DT_HASH).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Soft deepen (Wave 14 exclusive): soft stamp/caps/get/id/probe/touch +
 * cold inventory accessors. Extra dynsyms enrich DT_HASH; never hard-fail.
 * greppable: GJ_SO_SOFT_  /  libgj-so: soft
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Defined data export — GLOB_DAT / JUMP_SLOT resolve target (value 0x42). */
extern volatile uint64_t gj_so_export;

/** Optional init: restores gj_so_export / soft stamp / soft caps. */
void gj_so_init(void);

/** Function export: returns current gj_so_export (JUMP_SLOT-friendly). */
uint64_t gj_so_get_export(void);

/**
 * Classic System V ELF name hash (DT_HASH). Same algorithm as the
 * kernel / ld-gj SysV walkers. NULL name → 0.
 */
uint32_t gj_so_sysv_hash(const char *szName);

/** Canonical marker value for smokes / resolve checks. */
#define GJ_SO_EXPORT_VALUE          ((uint64_t)0x42)

/** Soft stamp value ('SYSV') — companion dynsym, not a smoke contract. */
#define GJ_SO_SOFT_STAMP_VALUE      ((uint64_t)0x53595356ull)

/** Soft capability bits present when soft deepen surface is linked. */
#define GJ_SO_SOFT_CAP_GET          ((uint32_t)0x1u)
#define GJ_SO_SOFT_CAP_ID           ((uint32_t)0x2u)
#define GJ_SO_SOFT_CAP_PROBE        ((uint32_t)0x4u)
#define GJ_SO_SOFT_CAP_TOUCH        ((uint32_t)0x8u)
#define GJ_SO_SOFT_CAP_HASH         ((uint32_t)0x10u)
#define GJ_SO_SOFT_CAP_MASK         ((uint32_t)0x1fu)

/** Wave 14 soft inventory stamp / area count (source greppable). */
#define GJ_SO_SOFT_WAVE_VALUE       14u
#define GJ_SO_SOFT_AREAS_VALUE      9u

/** Soft stamp data export — enriches DT_HASH; not a smoke contract. */
extern volatile uint64_t gj_so_soft_stamp;

/** Soft capability word (bitmask of soft deepen surface). */
extern volatile uint32_t gj_so_soft_caps;

/** Soft get: current product export value (read-only). */
uint64_t gj_so_soft_get(void);

/** Soft id: current soft stamp (identity for multi-SO soft probes). */
uint64_t gj_so_soft_id(void);

/**
 * Soft probe: 1 if export is canonical; on soft miss restores carefully
 * and returns 0. Never hard-fails.
 */
int gj_so_soft_probe(void);

/**
 * Soft touch: re-assert export + stamp + caps; returns canonical export.
 */
uint64_t gj_so_soft_touch(void);

/**
 * Cold soft inventory line (NUL-terminated static). Grep: libgj-so: soft
 */
const char *gj_so_soft_inventory(void);

/** Cold soft inventory: Wave stamp (14). */
unsigned gj_so_soft_wave(void);

/** Cold soft inventory: logical area count. */
unsigned gj_so_soft_areas(void);

/*
 * Optional soft batch scaffolds (src/graph_so_batch*.c) — not linked into
 * the product SO today. Declarations kept for future wire-up only.
 */

/* --- batch 1: SysV hash helpers + presence marker + soft deepen -------- */
extern volatile uint64_t gj_so_batch1_export;
extern volatile uint64_t gj_so_batch1_soft_stamp;
void gj_so_batch1_init(void);
uint64_t gj_so_batch1_id(void);
uint32_t gj_so_batch1_sysv_hash(const char *szName);
int gj_so_batch1_streq(const char *szA, const char *szB);
uint32_t gj_so_batch1_bucket(uint32_t u32Hash, uint32_t u32Nbucket);
uint64_t gj_so_batch1_soft_get(void);
int gj_so_batch1_soft_probe(void);

#define GJ_SO_BATCH1_EXPORT_VALUE       ((uint64_t)0x421)
#define GJ_SO_BATCH1_SOFT_STAMP_VALUE   ((uint64_t)0x42317331ull) /* 'B1s1' */

/* --- batch 2: multi-marker + soft chain walk + soft deepen ------------- */
extern volatile uint64_t gj_so_batch2_export_a;
extern volatile uint64_t gj_so_batch2_export_b;
extern volatile uint64_t gj_so_batch2_soft_stamp;
void gj_so_batch2_init(void);
uint64_t gj_so_batch2_id(void);
uint32_t gj_so_batch2_chain_next(const uint32_t *pChain, uint32_t u32Idx,
    uint32_t u32Nchain);
uint32_t gj_so_batch2_chain_find(const uint32_t *pChain, uint32_t u32Nchain,
    uint32_t u32Start, int (*pfnMatch)(uint32_t u32Idx, void *pCtx),
    void *pCtx);
uint64_t gj_so_batch2_soft_get(void);
int gj_so_batch2_soft_probe(void);

#define GJ_SO_BATCH2_EXPORT_A_VALUE     ((uint64_t)0x422)
#define GJ_SO_BATCH2_EXPORT_B_VALUE     ((uint64_t)0x423)
#define GJ_SO_BATCH2_SOFT_STAMP_VALUE   ((uint64_t)0x42327332ull) /* 'B2s2' */

#ifdef __cplusplus
}
#endif
