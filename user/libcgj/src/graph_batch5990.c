/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5990: bar3 aggregate score continuum + id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_agg_score_continuum_5990(void);
 *     - Returns 1 (soft compile-time product tag: bar3 aggregate ready
 *       score math continuum 5981-5990 is complete / ready).
 *   uint32_t gj_batch_id_5990(void);
 *     - Returns the compile-time graph batch number for this TU (5990).
 *   uint32_t __gj_bar3_agg_score_continuum_5990  (alias)
 *   uint32_t __gj_batch_id_5990  (alias)
 *   __libcgj_batch5990_marker = "libcgj-batch5990"
 *
 * Exclusive continuum CREATE-ONLY (5981-5990: bar3 aggregate ready
 * score math — score_clamp_5981, score_avg3_5982, score_sum3_5983,
 * score_min3_5984, score_max3_5985, mask_popscore_5986,
 * ready_threshold_5987, ready_score_p_5988,
 * aggregate_ready_score_5989, continuum + batch_id_5990). Unique
 * surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5990_marker[] = "libcgj-batch5990";

/* Continuum-complete lamp for the 5981-5990 exclusive wave. */
#define B5990_CONTINUUM_READY  1u
#define B5990_BATCH_ID         5990u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5990_continuum(void)
{
	return B5990_CONTINUUM_READY;
}

static uint32_t
b5990_id(void)
{
	return B5990_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_agg_score_continuum_5990 - continuum-ready tag for 5981-5990.
 *
 * Always returns 1. Soft pure-data product tag that the bar3 aggregate
 * ready score math exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_bar3_agg_score_continuum_5990(void)
{
	(void)NULL;
	return b5990_continuum();
}

/*
 * gj_batch_id_5990 - report this TU's graph batch number.
 *
 * Always returns 5990.
 */
uint32_t
gj_batch_id_5990(void)
{
	return b5990_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bar3_agg_score_continuum_5990(void)
    __attribute__((alias("gj_bar3_agg_score_continuum_5990")));

uint32_t __gj_batch_id_5990(void)
    __attribute__((alias("gj_batch_id_5990")));
