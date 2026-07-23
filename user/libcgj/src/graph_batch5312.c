/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5312: pure-value professional PCM rate 48 kHz (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_rate_48k_u(void);
 *     - Return the standard professional / broadcast sample rate in Hz
 *       (48000).
 *   uint32_t __gj_audio_rate_48k_u  (alias)
 *   __libcgj_batch5312_marker = "libcgj-batch5312"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Unique gj_audio_rate_48k_u
 * surface only; no multi-def. Distinct from gj_audio_rate_44k_u
 * (batch5311) and gj_audio_rate_prefer (batch2278). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5312_marker[] = "libcgj-batch5312";

/* Professional / broadcast PCM sample rate (Hz). */
#define B5312_RATE_48K  ((uint32_t)48000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5312_rate_48k(void)
{
	return B5312_RATE_48K;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_rate_48k_u - professional PCM sample rate constant (48000 Hz).
 *
 * Always returns 48000. Self-contained; no parent wires.
 */
uint32_t
gj_audio_rate_48k_u(void)
{
	(void)NULL;
	return b5312_rate_48k();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_rate_48k_u(void)
    __attribute__((alias("gj_audio_rate_48k_u")));
