/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4861: signed s16 PCM to unsigned u16 (bias).
 *
 * Surface (unique symbols):
 *   uint16_t gj_pcm_s16_to_u16_u(int16_t s);
 *     - Map signed PCM sample s into unsigned 16-bit by adding the
 *       mid-scale bias 32768 (two's-complement sign-bit flip). Full
 *       range: INT16_MIN → 0, 0 → 32768, INT16_MAX → 65535.
 *   uint16_t __gj_pcm_s16_to_u16_u  (alias)
 *   __libcgj_batch4861_marker = "libcgj-batch4861"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870: pcm_s16_to_u16_u,
 * pcm_u16_to_s16_u, pcm_clamp_s16_u, pcm_mix_s16_u,
 * pcm_mono_from_stereo_u, pcm_frame_bytes_u, pcm_byte_rate_u,
 * pcm_frames_u, pcm_duration_ms_u, batch_id_4870). Distinct from
 * gj_pcm_s16_to_u8_bias (batch1674) — unique gj_pcm_s16_to_u16_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4861_marker[] = "libcgj-batch4861";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bias s16 into full-range u16: out = (int32_t)s + 32768. */
static uint16_t
b4861_s16_to_u16(int16_t i16S)
{
	return (uint16_t)((int32_t)i16S + 32768);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_s16_to_u16_u - convert signed 16-bit PCM to unsigned 16-bit.
 *
 * s: signed PCM sample in [-32768, 32767]
 *
 * Returns u16 with mid-scale at 32768. Self-contained; no parent wires.
 */
uint16_t
gj_pcm_s16_to_u16_u(int16_t i16S)
{
	(void)NULL;
	return b4861_s16_to_u16(i16S);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_pcm_s16_to_u16_u(int16_t i16S)
    __attribute__((alias("gj_pcm_s16_to_u16_u")));
