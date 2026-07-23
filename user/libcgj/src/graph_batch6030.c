/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6030: MILESTONE 6030 batch identity + Top50
 * path continuum tag for the post-6000 path deepen wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6030(void);
 *     - Returns the compile-time graph batch number for this TU (6030).
 *   uint32_t gj_graph_milestone_6030(void);
 *     - Returns milestone revision 6030 (end of 6021-6030 wave).
 *   uint32_t gj_top50_path_continuum_6030(void);
 *     - Wave fingerprint: 0x1785178e (6021<<16 | 6030).
 *   uint32_t __gj_batch_id_6030  (alias)
 *   uint32_t __gj_graph_milestone_6030  (alias)
 *   uint32_t __gj_top50_path_continuum_6030  (alias)
 *   __libcgj_batch6030_marker = "libcgj-batch6030"
 *
 * MILESTONE 6030 for the exclusive continuum CREATE-ONLY wave
 * (batches 6021-6029: top50_path_rank_ok_6021, top50_path_rank_band_6022,
 * top50_path_weight_6023, top50_path_install_bit_6024,
 * top50_path_steam_bit_6025, top50_path_proton_bit_6026,
 * top50_path_stage_count_6027, top50_path_score_6028,
 * top50_path_threshold/ready_p_6029). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6030_marker[] = "libcgj-batch6030";

#define B6030_BATCH_ID   6030u
/* Wave fingerprint: start<<16 | end  →  6021<<16 | 6030 */
#define B6030_WAVE_FP    ((6021u << 16) | 6030u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6030_id(void)
{
	return B6030_BATCH_ID;
}

static uint32_t
b6030_wave(void)
{
	return B6030_WAVE_FP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6030 - report this TU's graph batch number.
 *
 * Always returns 6030.
 */
uint32_t
gj_batch_id_6030(void)
{
	(void)NULL;
	return b6030_id();
}

/*
 * gj_graph_milestone_6030 - report this TU's graph milestone revision.
 *
 * Always returns 6030 (MILESTONE 6030). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_6030(void)
{
	return b6030_id();
}

/*
 * gj_top50_path_continuum_6030 - post-6000 Top50 path deepen wave tag.
 *
 * Always returns 0x1785178e (6021<<16 | 6030). Soft continuum fingerprint
 * only. No parent wires.
 */
uint32_t
gj_top50_path_continuum_6030(void)
{
	return b6030_wave();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_6030(void)
    __attribute__((alias("gj_batch_id_6030")));

uint32_t __gj_graph_milestone_6030(void)
    __attribute__((alias("gj_graph_milestone_6030")));

uint32_t __gj_top50_path_continuum_6030(void)
    __attribute__((alias("gj_top50_path_continuum_6030")));
