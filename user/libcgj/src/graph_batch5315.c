/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5315: pure-value stereo channel count (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_ch_stereo_u(void);
 *     - Return the stereo (two-channel) PCM channel count (2).
 *   uint32_t __gj_audio_ch_stereo_u  (alias)
 *   __libcgj_batch5315_marker = "libcgj-batch5315"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Unique gj_audio_ch_stereo_u
 * surface only; no multi-def. Distinct from gj_audio_ch_mono_u
 * (batch5314). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5315_marker[] = "libcgj-batch5315";

/* Stereo PCM channel count. */
#define B5315_CH_STEREO  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5315_ch_stereo(void)
{
	return B5315_CH_STEREO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_ch_stereo_u - stereo PCM channel count constant (2).
 *
 * Always returns 2. Self-contained; no parent wires.
 */
uint32_t
gj_audio_ch_stereo_u(void)
{
	(void)NULL;
	return b5315_ch_stereo();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_ch_stereo_u(void)
    __attribute__((alias("gj_audio_ch_stereo_u")));
