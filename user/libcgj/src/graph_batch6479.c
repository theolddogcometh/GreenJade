/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6479: broadcast/stream bitrate budget gate.
 *
 * Surface (unique symbols):
 *   int gj_bstream_budget_gate_6479(uint32_t video_kbps,
 *                                   uint32_t audio_kbps,
 *                                   uint32_t overhead_kbps,
 *                                   uint32_t cap_kbps);
 *     - Return 1 if (video + audio + overhead) <= cap_kbps with
 *       saturating add (overflow → fail). cap_kbps of 0 always fails.
 *   int __gj_bstream_budget_gate_6479  (alias)
 *   __libcgj_batch6479_marker = "libcgj-batch6479"
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

const char __libcgj_batch6479_marker[] = "libcgj-batch6479";

#define B6479_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6479_gate(uint32_t u32Video, uint32_t u32Audio, uint32_t u32Over,
    uint32_t u32Cap)
{
	uint32_t u32Sum;

	if (u32Cap == 0u) {
		return 0;
	}
	u32Sum = u32Video;
	if (u32Sum > (B6479_U32_MAX - u32Audio)) {
		return 0;
	}
	u32Sum += u32Audio;
	if (u32Sum > (B6479_U32_MAX - u32Over)) {
		return 0;
	}
	u32Sum += u32Over;
	if (u32Sum > u32Cap) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_budget_gate_6479 - soft total bitrate budget gate.
 *
 * video_kbps:    video encode bitrate
 * audio_kbps:    audio encode bitrate
 * overhead_kbps: mux/container pad
 * cap_kbps:      allowed total (0 → fail)
 *
 * Returns 1 when video+audio+overhead <= cap, else 0.
 * Pure integer. Does not call libc. No parent wires.
 */
int
gj_bstream_budget_gate_6479(uint32_t video_kbps, uint32_t audio_kbps,
    uint32_t overhead_kbps, uint32_t cap_kbps)
{
	(void)NULL;
	return b6479_gate(video_kbps, audio_kbps, overhead_kbps, cap_kbps);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bstream_budget_gate_6479(uint32_t video_kbps, uint32_t audio_kbps,
    uint32_t overhead_kbps, uint32_t cap_kbps)
    __attribute__((alias("gj_bstream_budget_gate_6479")));
