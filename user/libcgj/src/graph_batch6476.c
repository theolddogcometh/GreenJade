/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6476: broadcast/stream audio bitrate gate.
 *
 * Surface (unique symbols):
 *   int gj_bstream_audio_bitrate_ok_6476(uint32_t audio_kbps);
 *     - Return 1 if audio_kbps is in soft audio encode band
 *       [32, 512] kbps, else 0.
 *   int __gj_bstream_audio_bitrate_ok_6476  (alias)
 *   __libcgj_batch6476_marker = "libcgj-batch6476"
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

const char __libcgj_batch6476_marker[] = "libcgj-batch6476";

/* Soft live-audio encode band (kbps). */
#define B6476_AUDIO_MIN  32u
#define B6476_AUDIO_MAX  512u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6476_audio_ok(uint32_t u32Kbps)
{
	if (u32Kbps < B6476_AUDIO_MIN) {
		return 0;
	}
	if (u32Kbps > B6476_AUDIO_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_audio_bitrate_ok_6476 - soft audio bitrate validity gate.
 *
 * audio_kbps: audio encode target in kilobits/sec.
 * Returns 1 when in [32, 512], else 0.
 * Does not open a codec. Does not call libc. No parent wires.
 */
int
gj_bstream_audio_bitrate_ok_6476(uint32_t audio_kbps)
{
	(void)NULL;
	return b6476_audio_ok(audio_kbps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bstream_audio_bitrate_ok_6476(uint32_t audio_kbps)
    __attribute__((alias("gj_bstream_audio_bitrate_ok_6476")));
