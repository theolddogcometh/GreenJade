/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5820: MILESTONE 5820 batch identity + wave
 * fingerprint for the product bar3 steam bootstrap finalize continuum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5820(void);
 *     - Returns the compile-time graph batch number for this TU (5820).
 *   uint32_t gj_graph_milestone_5820(void);
 *     - Returns milestone revision 5820 (end of 5811-5820 wave).
 *   uint32_t gj_product_bar3_steam_wave_5820(void);
 *     - Wave fingerprint: 0x16B316C4 (5811<<16 | 5820).
 *   uint32_t __gj_batch_id_5820  (alias)
 *   uint32_t __gj_graph_milestone_5820  (alias)
 *   uint32_t __gj_product_bar3_steam_wave_5820  (alias)
 *   __libcgj_batch5820_marker = "libcgj-batch5820"
 *
 * MILESTONE 5820 for the exclusive continuum CREATE-ONLY wave
 * (batches 5811-5819: product_bar3_boot_root_5811,
 * bar3_steam_boot_phase_5812, steam_bootstrap_ready_5813,
 * product_bootstrap_step_5814, bar3_bootstrap_finalize_ok_5815,
 * steam_bootstrap_finalize_5816, product_finalize_ready_5817,
 * bar3_finalize_score_5818, product_bar3_steam_finalize_ready_5819).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* or milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5820_marker[] = "libcgj-batch5820";

#define B5820_BATCH_ID     5820u
#define B5820_WAVE_START   5811u
#define B5820_WAVE_END     5820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5820_id(void)
{
	return B5820_BATCH_ID;
}

static uint32_t
b5820_wave(void)
{
	return (B5820_WAVE_START << 16) | B5820_WAVE_END;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5820 - report this TU's graph batch number.
 *
 * Always returns 5820.
 */
uint32_t
gj_batch_id_5820(void)
{
	(void)NULL;
	return b5820_id();
}

/*
 * gj_graph_milestone_5820 - report this TU's graph milestone revision.
 *
 * Always returns 5820 (MILESTONE 5820). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5820(void)
{
	return b5820_id();
}

/*
 * gj_product_bar3_steam_wave_5820 - wave fingerprint for 5811-5820.
 *
 * Returns (5811 << 16) | 5820. Soft pure-data fingerprint only.
 */
uint32_t
gj_product_bar3_steam_wave_5820(void)
{
	return b5820_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5820(void)
    __attribute__((alias("gj_batch_id_5820")));

uint32_t __gj_graph_milestone_5820(void)
    __attribute__((alias("gj_graph_milestone_5820")));

uint32_t __gj_product_bar3_steam_wave_5820(void)
    __attribute__((alias("gj_product_bar3_steam_wave_5820")));
