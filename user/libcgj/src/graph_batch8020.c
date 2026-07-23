/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8020: MILESTONE 8020 malloc-align wave end.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8020(void);
 *     - Returns the compile-time graph batch number for this TU (8020).
 *   uint32_t gj_malloc_align_wave_8020(void);
 *     - Wave fingerprint: (8011 << 16) | 8020.
 *   uint32_t gj_malloc_align_continuum_ready_8020(void);
 *     - Returns 1 (malloc-alignment stub continuum complete).
 *   uint32_t __gj_batch_id_8020  (alias)
 *   uint32_t __gj_malloc_align_wave_8020  (alias)
 *   uint32_t __gj_malloc_align_continuum_ready_8020  (alias)
 *   __libcgj_batch8020_marker = "libcgj-batch8020"
 *
 * MILESTONE 8020 for the exclusive continuum CREATE-ONLY wave
 * (batches 8011-8019: malloc_align default_const, is_pow2, align_up,
 * align_down, pad, posix_ok, chunk, ptr_align, clamp). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8020_marker[] = "libcgj-batch8020";

#define B8020_BATCH_ID    8020u
#define B8020_WAVE_START  8011u
#define B8020_WAVE_END    8020u
#define B8020_READY       1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8020_id(void)
{
	return B8020_BATCH_ID;
}

static uint32_t
b8020_wave(void)
{
	return (B8020_WAVE_START << 16) | B8020_WAVE_END;
}

static uint32_t
b8020_ready(void)
{
	return B8020_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8020 - report this TU's graph batch number.
 *
 * Always returns 8020.
 */
uint32_t
gj_batch_id_8020(void)
{
	(void)NULL;
	return b8020_id();
}

/*
 * gj_malloc_align_wave_8020 - wave fingerprint for 8011-8020.
 *
 * Returns (8011 << 16) | 8020. Soft pure-data fingerprint only.
 * No parent wires.
 */
uint32_t
gj_malloc_align_wave_8020(void)
{
	return b8020_wave();
}

/*
 * gj_malloc_align_continuum_ready_8020 - continuum ready tag.
 *
 * Always returns 1 (malloc alignment stub continuum complete).
 */
uint32_t
gj_malloc_align_continuum_ready_8020(void)
{
	return b8020_ready();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_8020(void)
    __attribute__((alias("gj_batch_id_8020")));

uint32_t __gj_malloc_align_wave_8020(void)
    __attribute__((alias("gj_malloc_align_wave_8020")));

uint32_t __gj_malloc_align_continuum_ready_8020(void)
    __attribute__((alias("gj_malloc_align_continuum_ready_8020")));
