/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5314: pure-value mono channel count (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_ch_mono_u(void);
 *     - Return the mono (single-channel) PCM channel count (1).
 *   uint32_t __gj_audio_ch_mono_u  (alias)
 *   __libcgj_batch5314_marker = "libcgj-batch5314"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Unique gj_audio_ch_mono_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5314_marker[] = "libcgj-batch5314";

/* Mono PCM channel count. */
#define B5314_CH_MONO  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5314_ch_mono(void)
{
	return B5314_CH_MONO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_ch_mono_u - mono PCM channel count constant (1).
 *
 * Always returns 1. Self-contained; no parent wires.
 */
uint32_t
gj_audio_ch_mono_u(void)
{
	(void)NULL;
	return b5314_ch_mono();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_ch_mono_u(void)
    __attribute__((alias("gj_audio_ch_mono_u")));
