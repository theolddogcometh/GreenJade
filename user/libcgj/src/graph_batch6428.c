/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6428: WebRTC voice jitter buffer budget stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_webrtc_jitter_budget_6428(uint32_t jitter_ms,
 *                                         uint32_t frame_ms);
 *     - Soft jitter buffer budget ms: jitter_ms + 2*frame_ms, saturating
 *       at 500 ms. Pure soft estimate for voice QoS stubs.
 *   uint32_t __gj_webrtc_jitter_budget_6428  (alias)
 *   __libcgj_batch6428_marker = "libcgj-batch6428"
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

const char __libcgj_batch6428_marker[] = "libcgj-batch6428";

#define B6428_BUDGET_CAP  500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6428_budget(uint32_t u32Jitter, uint32_t u32Frame)
{
	uint32_t u32Twice;
	uint32_t u32Sum;

	if (u32Frame > (B6428_BUDGET_CAP / 2u)) {
		return B6428_BUDGET_CAP;
	}
	u32Twice = u32Frame * 2u;
	if (u32Jitter > (B6428_BUDGET_CAP - u32Twice)) {
		return B6428_BUDGET_CAP;
	}
	u32Sum = u32Jitter + u32Twice;
	if (u32Sum > B6428_BUDGET_CAP) {
		return B6428_BUDGET_CAP;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_webrtc_jitter_budget_6428 - soft voice jitter buffer budget ms.
 *
 * jitter_ms: measured/estimated jitter in milliseconds
 * frame_ms:  codec frame duration in milliseconds
 *
 * Returns jitter + 2*frame, capped at 500. Pure integer. No parent wires.
 */
uint32_t
gj_webrtc_jitter_budget_6428(uint32_t jitter_ms, uint32_t frame_ms)
{
	(void)NULL;
	return b6428_budget(jitter_ms, frame_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_webrtc_jitter_budget_6428(uint32_t jitter_ms, uint32_t frame_ms)
    __attribute__((alias("gj_webrtc_jitter_budget_6428")));
