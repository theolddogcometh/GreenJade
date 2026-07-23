/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6426: Steam voice sample-rate soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_voice_rate_ok_6426(uint32_t sample_hz);
 *     - Soft Steam voice sample rate: 8000, 16000, 24000, or 48000 → 1;
 *       else 0.
 *   uint32_t __gj_steam_voice_rate_ok_6426  (alias)
 *   __libcgj_batch6426_marker = "libcgj-batch6426"
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

const char __libcgj_batch6426_marker[] = "libcgj-batch6426";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6426_rate_ok(uint32_t u32Hz)
{
	if (u32Hz == 8000u || u32Hz == 16000u || u32Hz == 24000u ||
	    u32Hz == 48000u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_voice_rate_ok_6426 - soft Steam voice sample-rate validity.
 *
 * sample_hz: sample rate in hertz
 *
 * Returns 1 for 8/16/24/48 kHz, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_steam_voice_rate_ok_6426(uint32_t sample_hz)
{
	(void)NULL;
	return b6426_rate_ok(sample_hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_voice_rate_ok_6426(uint32_t sample_hz)
    __attribute__((alias("gj_steam_voice_rate_ok_6426")));
