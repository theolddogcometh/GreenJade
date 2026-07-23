/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6424: WebRTC RTP Opus payload-type soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_webrtc_rtp_pt_opus_ok_6424(uint32_t payload_type);
 *     - Soft dynamic RTP PT range for Opus: payload_type in [96, 127]
 *       → 1; else 0.
 *   uint32_t __gj_webrtc_rtp_pt_opus_ok_6424  (alias)
 *   __libcgj_batch6424_marker = "libcgj-batch6424"
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

const char __libcgj_batch6424_marker[] = "libcgj-batch6424";

#define B6424_PT_MIN  96u
#define B6424_PT_MAX  127u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6424_pt_ok(uint32_t u32Pt)
{
	if (u32Pt >= B6424_PT_MIN && u32Pt <= B6424_PT_MAX) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_webrtc_rtp_pt_opus_ok_6424 - soft dynamic RTP payload type for Opus.
 *
 * payload_type: RTP PT value (0-127)
 *
 * Returns 1 when PT is in the dynamic range [96, 127], else 0. Pure
 * integer. No parent wires.
 */
uint32_t
gj_webrtc_rtp_pt_opus_ok_6424(uint32_t payload_type)
{
	(void)NULL;
	return b6424_pt_ok(payload_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_webrtc_rtp_pt_opus_ok_6424(uint32_t payload_type)
    __attribute__((alias("gj_webrtc_rtp_pt_opus_ok_6424")));
