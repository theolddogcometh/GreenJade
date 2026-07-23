/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6472: broadcast/stream CBR target gate.
 *
 * Surface (unique symbols):
 *   int gj_bstream_cbr_target_ok_6472(uint32_t target_kbps);
 *     - Return 1 if target_kbps is in soft CBR band [64, 50000] kbps,
 *       else 0.
 *   int __gj_bstream_cbr_target_ok_6472  (alias)
 *   __libcgj_batch6472_marker = "libcgj-batch6472"
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

const char __libcgj_batch6472_marker[] = "libcgj-batch6472";

/* Soft CBR target band for live/broadcast encode (kbps). */
#define B6472_CBR_MIN  64u
#define B6472_CBR_MAX  50000u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6472_cbr_ok(uint32_t u32Target)
{
	if (u32Target < B6472_CBR_MIN) {
		return 0;
	}
	if (u32Target > B6472_CBR_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_cbr_target_ok_6472 - soft CBR target validity gate.
 *
 * target_kbps: constant-bitrate encode target in kilobits/sec.
 * Returns 1 when in [64, 50000], else 0.
 * Does not start an encoder. Does not call libc. No parent wires.
 */
int
gj_bstream_cbr_target_ok_6472(uint32_t target_kbps)
{
	(void)NULL;
	return b6472_cbr_ok(target_kbps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bstream_cbr_target_ok_6472(uint32_t target_kbps)
    __attribute__((alias("gj_bstream_cbr_target_ok_6472")));
