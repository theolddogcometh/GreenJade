/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6427: Steam voice channel-count soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_voice_ch_ok_6427(uint32_t channels);
 *     - Soft channel count: mono (1) or stereo (2) → 1; else 0.
 *   uint32_t __gj_steam_voice_ch_ok_6427  (alias)
 *   __libcgj_batch6427_marker = "libcgj-batch6427"
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

const char __libcgj_batch6427_marker[] = "libcgj-batch6427";

#define B6427_CH_MONO    1u
#define B6427_CH_STEREO  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6427_ch_ok(uint32_t u32Ch)
{
	if (u32Ch == B6427_CH_MONO || u32Ch == B6427_CH_STEREO) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_voice_ch_ok_6427 - soft Steam voice channel-count validity.
 *
 * channels: channel count (1=mono, 2=stereo)
 *
 * Returns 1 for mono or stereo, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_steam_voice_ch_ok_6427(uint32_t channels)
{
	(void)NULL;
	return b6427_ch_ok(channels);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_voice_ch_ok_6427(uint32_t channels)
    __attribute__((alias("gj_steam_voice_ch_ok_6427")));
