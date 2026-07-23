/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6429: Steam voice packet soft quality score.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_voice_pkt_score_6429(uint32_t loss_pct,
 *                                          uint32_t rtt_ms);
 *     - Soft packet quality score [0, 100]: start 100, subtract
 *       loss_pct and rtt_ms/20 (integer), floor at 0.
 *   uint32_t __gj_steam_voice_pkt_score_6429  (alias)
 *   __libcgj_batch6429_marker = "libcgj-batch6429"
 *
 * Exclusive continuum CREATE-ONLY (6421-6430: WebRTC/steam voice stubs —
 * ice_host_ok_6421, dtls_fp_len_ok_6422, srtp_key_ok_6423,
 * rtp_pt_opus_ok_6424, opus_frame_ok_6425, steam_voice_rate_ok_6426,
 * steam_voice_ch_ok_6427, jitter_budget_6428, steam_voice_pkt_score_6429,
 * voice_ready_6430). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6429_marker[] = "libcgj-batch6429";

#define B6429_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6429_score(uint32_t u32Loss, uint32_t u32Rtt)
{
	uint32_t u32Pen;
	uint32_t u32RttPen;

	if (u32Loss > B6429_SCORE_MAX) {
		u32Loss = B6429_SCORE_MAX;
	}
	u32RttPen = u32Rtt / 20u;
	if (u32RttPen > B6429_SCORE_MAX) {
		u32RttPen = B6429_SCORE_MAX;
	}
	u32Pen = u32Loss + u32RttPen;
	if (u32Pen >= B6429_SCORE_MAX) {
		return 0u;
	}
	return B6429_SCORE_MAX - u32Pen;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_voice_pkt_score_6429 - soft Steam voice packet quality score.
 *
 * loss_pct: packet loss percent [0, 100+]
 * rtt_ms:   round-trip time in milliseconds
 *
 * Returns score in [0, 100]: 100 - loss_pct - (rtt_ms/20), floored at 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_steam_voice_pkt_score_6429(uint32_t loss_pct, uint32_t rtt_ms)
{
	(void)NULL;
	return b6429_score(loss_pct, rtt_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_voice_pkt_score_6429(uint32_t loss_pct, uint32_t rtt_ms)
    __attribute__((alias("gj_steam_voice_pkt_score_6429")));
