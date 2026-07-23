/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6422: WebRTC DTLS fingerprint length soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_webrtc_dtls_fp_len_ok_6422(uint32_t fp_len);
 *     - Soft DTLS fingerprint length check: sha-256 hex is 64 chars;
 *       accepts 40 (sha-1 hex) or 64 → 1; else 0.
 *   uint32_t __gj_webrtc_dtls_fp_len_ok_6422  (alias)
 *   __libcgj_batch6422_marker = "libcgj-batch6422"
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

const char __libcgj_batch6422_marker[] = "libcgj-batch6422";

#define B6422_FP_SHA1   40u
#define B6422_FP_SHA256 64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6422_fp_len_ok(uint32_t u32Len)
{
	if (u32Len == B6422_FP_SHA1 || u32Len == B6422_FP_SHA256) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_webrtc_dtls_fp_len_ok_6422 - soft DTLS fingerprint length validity.
 *
 * fp_len: fingerprint string length in characters (hex)
 *
 * Returns 1 for 40 (sha-1) or 64 (sha-256), else 0. Pure integer. No
 * parent wires.
 */
uint32_t
gj_webrtc_dtls_fp_len_ok_6422(uint32_t fp_len)
{
	(void)NULL;
	return b6422_fp_len_ok(fp_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_webrtc_dtls_fp_len_ok_6422(uint32_t fp_len)
    __attribute__((alias("gj_webrtc_dtls_fp_len_ok_6422")));
