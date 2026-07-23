/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6474: broadcast/stream ABR ladder rung (kbps).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bstream_abr_rung_kbps_6474(uint32_t avail_kbps,
 *                                          uint32_t rung_count);
 *     - Soft ABR rung pick from a fixed 5-step ladder
 *       {500, 1500, 3000, 6000, 12000} kbps. Returns the highest rung
 *       that is <= avail_kbps and within rung_count (1..5, 0→5).
 *       If avail_kbps is below the first rung, returns 0.
 *   uint32_t __gj_bstream_abr_rung_kbps_6474  (alias)
 *   __libcgj_batch6474_marker = "libcgj-batch6474"
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

const char __libcgj_batch6474_marker[] = "libcgj-batch6474";

#define B6474_LADDER_N  5u

/* Soft fixed ABR ladder (kbps), low → high. */
static const uint32_t g_b6474_ladder[B6474_LADDER_N] = {
	500u, 1500u, 3000u, 6000u, 12000u
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6474_rung(uint32_t u32Avail, uint32_t u32Count)
{
	uint32_t u32N;
	uint32_t u32Best;
	uint32_t iRung;

	u32N = u32Count;
	if (u32N == 0u || u32N > B6474_LADDER_N) {
		u32N = B6474_LADDER_N;
	}
	u32Best = 0u;
	for (iRung = 0u; iRung < u32N; iRung++) {
		if (g_b6474_ladder[iRung] <= u32Avail) {
			u32Best = g_b6474_ladder[iRung];
		}
	}
	return u32Best;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_abr_rung_kbps_6474 - pick soft ABR ladder rung (kbps).
 *
 * avail_kbps: measured available throughput in kilobits/sec
 * rung_count: how many ladder steps to consider (1..5; 0 → all 5)
 *
 * Returns highest ladder rung <= avail_kbps, or 0 if none fit.
 * Pure integer. Does not call libc. No parent wires.
 */
uint32_t
gj_bstream_abr_rung_kbps_6474(uint32_t avail_kbps, uint32_t rung_count)
{
	(void)NULL;
	return b6474_rung(avail_kbps, rung_count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bstream_abr_rung_kbps_6474(uint32_t avail_kbps,
    uint32_t rung_count)
    __attribute__((alias("gj_bstream_abr_rung_kbps_6474")));
