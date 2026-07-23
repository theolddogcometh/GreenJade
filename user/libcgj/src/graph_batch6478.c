/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6478: broadcast/stream mux overhead (kbps).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bstream_mux_overhead_kbps_6478(uint32_t media_kbps,
 *                                              uint32_t pct);
 *     - Soft container/mux overhead pad: media_kbps * pct / 100,
 *       with pct clamped to [0, 25] and product saturating on overflow.
 *   uint32_t __gj_bstream_mux_overhead_kbps_6478  (alias)
 *   __libcgj_batch6478_marker = "libcgj-batch6478"
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

const char __libcgj_batch6478_marker[] = "libcgj-batch6478";

#define B6478_PCT_MAX   25u
#define B6478_PCT_DIV  100u
#define B6478_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6478_overhead(uint32_t u32Media, uint32_t u32Pct)
{
	uint32_t u32P;
	uint32_t u32Prod;

	u32P = u32Pct;
	if (u32P > B6478_PCT_MAX) {
		u32P = B6478_PCT_MAX;
	}
	if (u32P == 0u || u32Media == 0u) {
		return 0u;
	}
	if (u32Media > (B6478_U32_MAX / u32P)) {
		return B6478_U32_MAX / B6478_PCT_DIV;
	}
	u32Prod = u32Media * u32P;
	return u32Prod / B6478_PCT_DIV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_mux_overhead_kbps_6478 - soft mux/container overhead pad.
 *
 * media_kbps: sum of audio+video media bitrate (kbps)
 * pct:        overhead percent (clamped to [0, 25])
 *
 * Returns media_kbps * pct / 100. Pure integer. No parent wires.
 */
uint32_t
gj_bstream_mux_overhead_kbps_6478(uint32_t media_kbps, uint32_t pct)
{
	(void)NULL;
	return b6478_overhead(media_kbps, pct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bstream_mux_overhead_kbps_6478(uint32_t media_kbps,
    uint32_t pct) __attribute__((alias("gj_bstream_mux_overhead_kbps_6478")));
