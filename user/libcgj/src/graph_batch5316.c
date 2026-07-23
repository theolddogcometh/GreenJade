/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5316: pure-value 16-bit sample depth (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_bps_16_u(void);
 *     - Return bits-per-sample for 16-bit PCM (16).
 *   uint32_t __gj_audio_bps_16_u  (alias)
 *   __libcgj_batch5316_marker = "libcgj-batch5316"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Unique gj_audio_bps_16_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5316_marker[] = "libcgj-batch5316";

/* 16-bit PCM sample depth (bits per sample). */
#define B5316_BPS_16  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5316_bps_16(void)
{
	return B5316_BPS_16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_bps_16_u - 16-bit PCM bits-per-sample constant (16).
 *
 * Always returns 16. Self-contained; no parent wires.
 */
uint32_t
gj_audio_bps_16_u(void)
{
	(void)NULL;
	return b5316_bps_16();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_bps_16_u(void)
    __attribute__((alias("gj_audio_bps_16_u")));
