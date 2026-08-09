/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Public surface of product SO libgj-gnu.so.1 (DT_GNU_HASH).
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Soft residual deepen (C2 lib residual; Soft!=product; G-AC-1;
 * dual MIT OR Apache-2.0; Dual DoD A/B OPEN; stamp-free bar honesty):
 *   product contract  — gj_gnu_export == 0x43; gj_gnu_init restores export
 *                       (GLOB_DAT / JUMP_SLOT smoke targets; stable)
 *   soft stamp/caps   — companion dynsyms enrich .gnu.hash; never smoke AC
 *   soft get/id       — read-only soft probes; Soft!=product identity only
 *   soft probe/touch  — careful restore on miss; never hard-fail product path
 *   soft inventory    — cold greppable residual catalog (wave/surfaces/areas)
 *   soft deepen/path  — residual honesty lines; not bar3 / Dual DoD close
 *   soft note         — cold call counters; wrap OK; observation only
 *   batch1 scaffold   — optional src/graph_gnu_batch1.c; not linked today
 *   G-AC-1            — this SO is freestanding dual-license ELF, not a
 *                       Linux .ko product AC; no GPL source in tree
 *   Dual DoD A/B      — both OPEN until DUT proof; soft dynsym residual
 *                       lamps never close Dual DoD A (USB UDX) or B (NIC UDX)
 *   freestanding SKIP — kernel class drivers SKIP default; product path =
 *                       userspace UDX+ABI (not this SO's soft surface)
 *   stamp-free        — Bar honesty v2026.08.04.75. NEVER bump
 *                       GJ_IMAGE_VERSION from this unit. Not image stamp.
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = stamp/caps/get/id/probe/touch + inventory/deepen/path/
 *               wave/surface/area/note residual (extra .gnu.hash dynsyms)
 *   product   = gj_gnu_export 0x43 + gj_gnu_init resolve/smoke contract
 *   Soft!=product: soft PASS / inventory / deepen != product AC != Dual DoD
 *
 * greppable: libgj-gnu: soft residual Soft!=product
 * greppable: libgj-gnu: soft residual G-AC-1
 * greppable: libgj-gnu: soft residual dual_dod OPEN
 * greppable: libgj-gnu: soft residual product_export=0x43
 * greppable: libgj-gnu: soft residual stamp-free
 * greppable: GJ_GNU_SOFT_
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN. Soft!=product.
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

/**
 * Soft residual inventory stamp (eng only; Soft!=product; not image stamp).
 * Not GJ_IMAGE_VERSION. Stamp-free bar honesty v2026.08.04.75.
 * greppable: libgj-gnu: soft residual Soft!=product
 */
#define GJ_GNU_SOFT_WAVE            70u
#define GJ_GNU_SOFT_SURFACES        12u
#define GJ_GNU_SOFT_AREAS           12u

/** Defined data export — GLOB_DAT / JUMP_SLOT resolve target (value 0x43). */
extern volatile uint64_t gj_gnu_export;

/** Soft stamp data export — enriches .gnu.hash; not a smoke contract. */
extern volatile uint64_t gj_gnu_soft_stamp;

/** Soft capability word (bitmask of soft deepen surface). */
extern volatile uint32_t gj_gnu_soft_caps;

/**
 * Optional init: restores gj_gnu_export / soft stamp / soft caps.
 * Product path re-store only — Soft!=product residual never rewrites AC.
 */
void gj_gnu_init(void);

/**
 * Soft get: current product export value (read-only).
 * Soft!=product: observation only; not Dual DoD close.
 */
uint64_t gj_gnu_soft_get(void);

/**
 * Soft id: current soft stamp (identity for multi-SO soft probes).
 * Soft!=product: soft identity != product export contract.
 */
uint64_t gj_gnu_soft_id(void);

/**
 * Soft probe: 1 if export is canonical; on soft miss restores carefully
 * and returns 0. Never hard-fails.
 * Soft!=product; G-AC-1: not a .ko product AC lamp.
 */
int gj_gnu_soft_probe(void);

/**
 * Soft touch: re-assert export + stamp + caps; returns canonical export.
 * Soft!=product: careful re-store; not Dual DoD A/B close.
 */
uint64_t gj_gnu_soft_touch(void);

/**
 * Cold soft inventory line (NUL-terminated static).
 * Soft!=product residual catalog. Grep: libgj-gnu: soft inventory
 * greppable: libgj-gnu: soft residual Soft!=product
 */
const char *gj_gnu_soft_inventory(void);

/**
 * Cold soft deepen stamp line.
 * Soft!=product. Grep: libgj-gnu: soft deepen
 */
const char *gj_gnu_soft_deepen(void);

/**
 * Cold soft path honesty line (hash=gnu soname).
 * Soft!=product. Grep: libgj-gnu: soft path
 */
const char *gj_gnu_soft_path(void);

/**
 * Cold soft honesty line (multi_server=0 confine=0 exclusive).
 * Soft!=product; Dual DoD A/B remain OPEN. Grep: libgj-gnu: soft honesty
 */
const char *gj_gnu_soft_honesty(void);

/**
 * Soft wave stamp (GJ_GNU_SOFT_WAVE). Soft!=product; not GJ_IMAGE_VERSION.
 * greppable: GJ_GNU_SOFT_WAVE
 */
uint32_t gj_gnu_soft_wave(void);

/**
 * Soft dynsym surface count for .gnu.hash soft catalog.
 * Soft!=product. greppable: GJ_GNU_SOFT_SURFACE
 */
uint32_t gj_gnu_soft_surface_count(void);

/**
 * Soft residual area catalog size.
 * Soft!=product. Grep: libgj-gnu: soft areas=
 */
uint32_t gj_gnu_soft_area_count(void);

/**
 * Soft residual area name by index (0..areas-1), or NULL.
 * Soft!=product. Grep: libgj-gnu: soft areas=
 */
const char *gj_gnu_soft_area_name(uint32_t uArea);

/**
 * Soft note rollup: packed get|id|probe|touch|inv|deepen|path counters.
 * Never hard-fails; wrap OK. Soft!=product observation only.
 * greppable: GJ_GNU_SOFT_NOTE
 */
uint64_t gj_gnu_soft_note(void);

/**
 * Soft note raw counters (get, id, probe, touch, inv) — optional out args.
 * NULL out pointers are soft-skipped. Soft!=product.
 */
void gj_gnu_soft_note_counts(uint32_t *pGet, uint32_t *pId, uint32_t *pProbe,
                             uint32_t *pTouch, uint32_t *pInv);

/**
 * Soft note extend: deepen + path call counters (optional out args).
 * Soft!=product residual. Grep: libgj-gnu: soft note
 */
void gj_gnu_soft_note_counts_ex(uint32_t *pDeepen, uint32_t *pPath);

/*
 * Optional batch scaffold (src/graph_gnu_batch1.c) — not linked into the
 * product SO today. Declarations kept for future wire-up only.
 * Soft!=product: batch markers != shipped product smoke AC.
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
