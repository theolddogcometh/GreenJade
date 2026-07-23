/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1674: signed 16-bit PCM → unsigned 8-bit biased.
 *
 * Surface (unique symbols):
 *   uint8_t gj_pcm_s16_to_u8_bias(int16_t s);
 *     — Inverse of gj_pcm_u8_bias_to_s16: take high byte of s16, add
 *       bias 128, clamp to [0, 255].
 *   uint8_t __gj_pcm_s16_to_u8_bias  (alias)
 *   __libcgj_batch1674_marker = "libcgj-batch1674"
 *
 * Audio/sample exclusive integer wave. Distinct from batch1673.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1674_marker[] = "libcgj-batch1674";

#define B1674_U8_BIAS  128

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b1674_clamp_u8(int32_t i32V)
{
	if (i32V < 0) {
		return 0u;
	}
	if (i32V > 255) {
		return 255u;
	}
	return (uint8_t)i32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_s16_to_u8_bias — quantize s16 PCM to biased unsigned 8-bit.
 *
 * Arithmetic right-shift by 8 recovers the high byte; +128 restores the
 * mid-scale bias used by classic 8-bit PCM containers.
 */
uint8_t
gj_pcm_s16_to_u8_bias(int16_t i16S)
{
	int32_t i32V;

	i32V = ((int32_t)i16S >> 8) + B1674_U8_BIAS;
	return b1674_clamp_u8(i32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_pcm_s16_to_u8_bias(int16_t i16S)
    __attribute__((alias("gj_pcm_s16_to_u8_bias")));
