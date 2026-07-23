/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6310: MILESTONE 6310 batch identity + wave
 * fingerprint for the post-6300 continuum product deepen continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6310(void);
 *     - Returns the compile-time graph batch number for this TU (6310).
 *   uint32_t gj_graph_milestone_6310(void);
 *     - Returns milestone revision 6310 (end of 6301-6310 wave).
 *   uint32_t gj_continuum_product_deepen_wave_6310(void);
 *     - Wave fingerprint: 0x189D18A6 (6301<<16 | 6310).
 *   uint32_t __gj_batch_id_6310  (alias)
 *   uint32_t __gj_graph_milestone_6310  (alias)
 *   uint32_t __gj_continuum_product_deepen_wave_6310  (alias)
 *   __libcgj_batch6310_marker = "libcgj-batch6310"
 *
 * MILESTONE 6310 for the exclusive continuum CREATE-ONLY wave
 * (batches 6301-6309: continuum_product_deepen_root_6301,
 * continuum_product_deepen_phase_6302, continuum_product_deepen_ready_6303,
 * continuum_product_deepen_step_6304, continuum_product_deepen_ok_6305,
 * continuum_product_deepen_code_6306,
 * continuum_product_deepen_finalize_ready_6307,
 * continuum_product_deepen_score_6308,
 * continuum_product_deepen_continuum_ready_6309). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6310_marker[] = "libcgj-batch6310";

#define B6310_BATCH_ID     6310u
#define B6310_WAVE_START   6301u
#define B6310_WAVE_END     6310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6310_id(void)
{
	return B6310_BATCH_ID;
}

static uint32_t
b6310_wave(void)
{
	return (B6310_WAVE_START << 16) | B6310_WAVE_END;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6310 - report this TU's graph batch number.
 *
 * Always returns 6310.
 */
uint32_t
gj_batch_id_6310(void)
{
	(void)NULL;
	return b6310_id();
}

/*
 * gj_graph_milestone_6310 - report this TU's graph milestone revision.
 *
 * Always returns 6310 (MILESTONE 6310). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6310(void)
{
	return b6310_id();
}

/*
 * gj_continuum_product_deepen_wave_6310 - wave fingerprint for 6301-6310.
 *
 * Returns (6301 << 16) | 6310. Soft pure-data fingerprint only.
 */
uint32_t
gj_continuum_product_deepen_wave_6310(void)
{
	return b6310_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6310(void)
    __attribute__((alias("gj_batch_id_6310")));

uint32_t __gj_graph_milestone_6310(void)
    __attribute__((alias("gj_graph_milestone_6310")));

uint32_t __gj_continuum_product_deepen_wave_6310(void)
    __attribute__((alias("gj_continuum_product_deepen_wave_6310")));
