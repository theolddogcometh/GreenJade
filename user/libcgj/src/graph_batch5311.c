/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5311: pure-value CD-quality PCM rate 44.1 kHz (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_rate_44k_u(void);
 *     - Return the standard CD-audio sample rate in Hz (44100).
 *   uint32_t __gj_audio_rate_44k_u  (alias)
 *   __libcgj_batch5311_marker = "libcgj-batch5311"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Unique gj_audio_rate_44k_u
 * surface only; no multi-def. Distinct from gj_audio_rate_prefer
 * (batch2278). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5311_marker[] = "libcgj-batch5311";

/* CD-quality PCM sample rate (Hz). */
#define B5311_RATE_44K  ((uint32_t)44100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5311_rate_44k(void)
{
	return B5311_RATE_44K;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_rate_44k_u - CD-quality PCM sample rate constant (44100 Hz).
 *
 * Always returns 44100. Self-contained; no parent wires.
 */
uint32_t
gj_audio_rate_44k_u(void)
{
	(void)NULL;
	return b5311_rate_44k();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_rate_44k_u(void)
    __attribute__((alias("gj_audio_rate_44k_u")));
