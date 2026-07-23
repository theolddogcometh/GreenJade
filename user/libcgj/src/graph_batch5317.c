/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5317: pure-value 24-bit sample depth (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_audio_bps_24_u(void);
 *     - Return bits-per-sample for 24-bit PCM (24).
 *   uint32_t __gj_audio_bps_24_u  (alias)
 *   __libcgj_batch5317_marker = "libcgj-batch5317"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Unique gj_audio_bps_24_u
 * surface only; no multi-def. Distinct from gj_audio_bps_16_u
 * (batch5316). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5317_marker[] = "libcgj-batch5317";

/* 24-bit PCM sample depth (bits per sample). */
#define B5317_BPS_24  ((uint32_t)24u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5317_bps_24(void)
{
	return B5317_BPS_24;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_audio_bps_24_u - 24-bit PCM bits-per-sample constant (24).
 *
 * Always returns 24. Self-contained; no parent wires.
 */
uint32_t
gj_audio_bps_24_u(void)
{
	(void)NULL;
	return b5317_bps_24();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_audio_bps_24_u(void)
    __attribute__((alias("gj_audio_bps_24_u")));
