/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7310: MILESTONE 7310 batch identity + wave
 * fingerprint for the post-7300 bar3 product deepen continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7310(void);
 *     - Returns the compile-time graph batch number for this TU (7310).
 *   uint32_t gj_graph_milestone_7310(void);
 *     - Returns milestone revision 7310 (end of 7301-7310 wave).
 *   uint32_t gj_bar3_product_deepen_wave_7310(void);
 *     - Wave fingerprint: 0x1C851C8E (7301<<16 | 7310).
 *   uint32_t __gj_batch_id_7310  (alias)
 *   uint32_t __gj_graph_milestone_7310  (alias)
 *   uint32_t __gj_bar3_product_deepen_wave_7310  (alias)
 *   __libcgj_batch7310_marker = "libcgj-batch7310"
 *
 * MILESTONE 7310 for the exclusive continuum CREATE-ONLY wave
 * (batches 7301-7309: bar3_product_deepen_root_7301,
 * bar3_product_deepen_phase_7302, bar3_product_deepen_ready_7303,
 * bar3_product_deepen_step_7304, bar3_product_deepen_ok_7305,
 * bar3_product_deepen_code_7306,
 * bar3_product_deepen_finalize_ready_7307,
 * bar3_product_deepen_score_7308,
 * bar3_product_deepen_continuum_ready_7309). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7310_marker[] = "libcgj-batch7310";

#define B7310_BATCH_ID     7310u
#define B7310_WAVE_START   7301u
#define B7310_WAVE_END     7310u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7310_id(void)
{
	return B7310_BATCH_ID;
}

static uint32_t
b7310_wave(void)
{
	return (B7310_WAVE_START << 16) | B7310_WAVE_END;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7310 - report this TU's graph batch number.
 *
 * Always returns 7310.
 */
uint32_t
gj_batch_id_7310(void)
{
	(void)NULL;
	return b7310_id();
}

/*
 * gj_graph_milestone_7310 - report this TU's graph milestone revision.
 *
 * Always returns 7310 (MILESTONE 7310). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_7310(void)
{
	return b7310_id();
}

/*
 * gj_bar3_product_deepen_wave_7310 - wave fingerprint for 7301-7310.
 *
 * Returns (7301 << 16) | 7310. Soft pure-data fingerprint only.
 */
uint32_t
gj_bar3_product_deepen_wave_7310(void)
{
	return b7310_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7310(void)
    __attribute__((alias("gj_batch_id_7310")));

uint32_t __gj_graph_milestone_7310(void)
    __attribute__((alias("gj_graph_milestone_7310")));

uint32_t __gj_bar3_product_deepen_wave_7310(void)
    __attribute__((alias("gj_bar3_product_deepen_wave_7310")));
