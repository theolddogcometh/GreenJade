/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6421: WebRTC ICE host candidate soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_webrtc_ice_host_ok_6421(uint32_t cand_type);
 *     - Soft ICE candidate type check: cand_type == 1 (host) → 1;
 *       else 0. Type codes: 1=host, 2=srflx, 3=prflx, 4=relay.
 *   uint32_t __gj_webrtc_ice_host_ok_6421  (alias)
 *   __libcgj_batch6421_marker = "libcgj-batch6421"
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

const char __libcgj_batch6421_marker[] = "libcgj-batch6421";

#define B6421_ICE_HOST  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6421_ice_host_ok(uint32_t u32Type)
{
	if (u32Type == B6421_ICE_HOST) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_webrtc_ice_host_ok_6421 - soft WebRTC ICE host candidate type check.
 *
 * cand_type: soft ICE candidate type code (1=host, 2=srflx, 3=prflx,
 *            4=relay)
 *
 * Returns 1 when cand_type is host (1), else 0. Pure integer. No parent
 * wires.
 */
uint32_t
gj_webrtc_ice_host_ok_6421(uint32_t cand_type)
{
	(void)NULL;
	return b6421_ice_host_ok(cand_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_webrtc_ice_host_ok_6421(uint32_t cand_type)
    __attribute__((alias("gj_webrtc_ice_host_ok_6421")));
