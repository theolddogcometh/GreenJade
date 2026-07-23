/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6423: WebRTC SRTP key length soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_webrtc_srtp_key_ok_6423(uint32_t key_len, uint32_t salt_len);
 *     - Soft SRTP master-key check: key_len == 16 and salt_len == 14
 *       (AES-CM-128 style soft sizes) → 1; else 0.
 *   uint32_t __gj_webrtc_srtp_key_ok_6423  (alias)
 *   __libcgj_batch6423_marker = "libcgj-batch6423"
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

const char __libcgj_batch6423_marker[] = "libcgj-batch6423";

#define B6423_KEY_LEN   16u
#define B6423_SALT_LEN  14u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6423_srtp_key_ok(uint32_t u32Key, uint32_t u32Salt)
{
	if (u32Key == B6423_KEY_LEN && u32Salt == B6423_SALT_LEN) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_webrtc_srtp_key_ok_6423 - soft SRTP master key/salt length check.
 *
 * key_len:  soft master key length in bytes
 * salt_len: soft master salt length in bytes
 *
 * Returns 1 when key_len==16 and salt_len==14, else 0. Pure integer.
 * No parent wires.
 */
uint32_t
gj_webrtc_srtp_key_ok_6423(uint32_t key_len, uint32_t salt_len)
{
	(void)NULL;
	return b6423_srtp_key_ok(key_len, salt_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_webrtc_srtp_key_ok_6423(uint32_t key_len, uint32_t salt_len)
    __attribute__((alias("gj_webrtc_srtp_key_ok_6423")));
