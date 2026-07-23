/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6475: broadcast/stream bytes/sec → kbps.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bstream_bytes_to_kbps_6475(uint32_t bytes_per_sec);
 *     - Convert a measured byte rate into kilobits/sec:
 *       (bytes_per_sec * 8) / 1000, saturating at UINT32_MAX on
 *       overflow of the *8 step.
 *   uint32_t __gj_bstream_bytes_to_kbps_6475  (alias)
 *   __libcgj_batch6475_marker = "libcgj-batch6475"
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

const char __libcgj_batch6475_marker[] = "libcgj-batch6475";

#define B6475_BITS_PER_BYTE  8u
#define B6475_KILO           1000u
#define B6475_U32_MAX        0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6475_to_kbps(uint32_t u32Bps)
{
	uint32_t u32Bits;

	/* Saturate bytes*8 if it would overflow u32. */
	if (u32Bps > (B6475_U32_MAX / B6475_BITS_PER_BYTE)) {
		return B6475_U32_MAX / B6475_KILO;
	}
	u32Bits = u32Bps * B6475_BITS_PER_BYTE;
	return u32Bits / B6475_KILO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_bytes_to_kbps_6475 - convert byte rate to kilobits/sec.
 *
 * bytes_per_sec: measured throughput in bytes per second.
 * Returns (bytes_per_sec * 8) / 1000 with soft overflow saturation.
 * Pure integer. Does not call libc. No parent wires.
 */
uint32_t
gj_bstream_bytes_to_kbps_6475(uint32_t bytes_per_sec)
{
	(void)NULL;
	return b6475_to_kbps(bytes_per_sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bstream_bytes_to_kbps_6475(uint32_t bytes_per_sec)
    __attribute__((alias("gj_bstream_bytes_to_kbps_6475")));
