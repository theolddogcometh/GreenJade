/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6471: broadcast/stream bitrate clamp (kbps).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bstream_bitrate_clamp_kbps_6471(uint32_t kbps,
 *                                               uint32_t min_kbps,
 *                                               uint32_t max_kbps);
 *     - Clamp kbps into [min_kbps, max_kbps]. If min_kbps > max_kbps,
 *       swap soft bounds first. Pure integer clamp.
 *   uint32_t __gj_bstream_bitrate_clamp_kbps_6471  (alias)
 *   __libcgj_batch6471_marker = "libcgj-batch6471"
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

const char __libcgj_batch6471_marker[] = "libcgj-batch6471";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6471_clamp(uint32_t u32Kbps, uint32_t u32Min, uint32_t u32Max)
{
	uint32_t u32Lo;
	uint32_t u32Hi;

	u32Lo = u32Min;
	u32Hi = u32Max;
	if (u32Lo > u32Hi) {
		u32Lo = u32Max;
		u32Hi = u32Min;
	}
	if (u32Kbps < u32Lo) {
		return u32Lo;
	}
	if (u32Kbps > u32Hi) {
		return u32Hi;
	}
	return u32Kbps;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_bitrate_clamp_kbps_6471 - clamp stream bitrate to [min, max].
 *
 * kbps:     candidate bitrate in kilobits/sec
 * min_kbps: soft lower bound
 * max_kbps: soft upper bound
 *
 * Returns kbps clamped into the soft band (bounds swapped if inverted).
 * Pure integer. Does not call libc. No parent wires.
 */
uint32_t
gj_bstream_bitrate_clamp_kbps_6471(uint32_t kbps, uint32_t min_kbps,
    uint32_t max_kbps)
{
	(void)NULL;
	return b6471_clamp(kbps, min_kbps, max_kbps);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bstream_bitrate_clamp_kbps_6471(uint32_t kbps,
    uint32_t min_kbps, uint32_t max_kbps)
    __attribute__((alias("gj_bstream_bitrate_clamp_kbps_6471")));
