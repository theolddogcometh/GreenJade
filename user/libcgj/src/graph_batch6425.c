/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6425: WebRTC Opus frame duration soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_webrtc_opus_frame_ok_6425(uint32_t frame_ms);
 *     - Soft Opus frame duration check: 2, 5, 10, 20, 40, or 60 ms → 1;
 *       else 0.
 *   uint32_t __gj_webrtc_opus_frame_ok_6425  (alias)
 *   __libcgj_batch6425_marker = "libcgj-batch6425"
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

const char __libcgj_batch6425_marker[] = "libcgj-batch6425";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6425_frame_ok(uint32_t u32Ms)
{
	if (u32Ms == 2u || u32Ms == 5u || u32Ms == 10u || u32Ms == 20u ||
	    u32Ms == 40u || u32Ms == 60u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_webrtc_opus_frame_ok_6425 - soft Opus frame duration validity.
 *
 * frame_ms: frame duration in milliseconds
 *
 * Returns 1 for standard Opus durations (2/5/10/20/40/60), else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_webrtc_opus_frame_ok_6425(uint32_t frame_ms)
{
	(void)NULL;
	return b6425_frame_ok(frame_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_webrtc_opus_frame_ok_6425(uint32_t frame_ms)
    __attribute__((alias("gj_webrtc_opus_frame_ok_6425")));
