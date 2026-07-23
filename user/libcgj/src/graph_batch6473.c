/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6473: broadcast/stream VBR range gate.
 *
 * Surface (unique symbols):
 *   int gj_bstream_vbr_range_ok_6473(uint32_t min_kbps, uint32_t max_kbps);
 *     - Return 1 if min_kbps > 0, max_kbps >= min_kbps, and max_kbps
 *       <= 100000 (soft VBR band). Else 0.
 *   int __gj_bstream_vbr_range_ok_6473  (alias)
 *   __libcgj_batch6473_marker = "libcgj-batch6473"
 *
 * Exclusive continuum CREATE-ONLY (6471-6480: broadcast/streaming
 * bitrate stubs — bitrate_clamp_kbps_6471, cbr_target_ok_6472,
 * vbr_range_ok_6473, abr_rung_kbps_6474, bytes_to_kbps_6475,
 * audio_bitrate_ok_6476, video_budget_kbps_6477, mux_overhead_kbps_6478,
 * budget_gate_6479, continuum + batch_id_6480). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6473_marker[] = "libcgj-batch6473";

/* Soft absolute VBR ceiling for live/broadcast product policy (kbps). */
#define B6473_VBR_ABS_MAX  100000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6473_vbr_ok(uint32_t u32Min, uint32_t u32Max)
{
	if (u32Min == 0u) {
		return 0;
	}
	if (u32Max < u32Min) {
		return 0;
	}
	if (u32Max > B6473_VBR_ABS_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_vbr_range_ok_6473 - soft VBR min/max range gate.
 *
 * min_kbps: variable-bitrate floor (must be non-zero)
 * max_kbps: variable-bitrate ceiling (must be >= min, <= 100000)
 *
 * Returns 1 when range is soft-valid, else 0. Pure integer.
 * Does not call libc. No parent wires.
 */
int
gj_bstream_vbr_range_ok_6473(uint32_t min_kbps, uint32_t max_kbps)
{
	(void)NULL;
	return b6473_vbr_ok(min_kbps, max_kbps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bstream_vbr_range_ok_6473(uint32_t min_kbps, uint32_t max_kbps)
    __attribute__((alias("gj_bstream_vbr_range_ok_6473")));
