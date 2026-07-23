/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1673: unsigned 8-bit biased PCM → signed 16-bit.
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_u8_bias_to_s16(uint8_t u);
 *     — Map u8 PCM with mid-scale bias 128 into s16:
 *       ((int32_t)u - 128) << 8. Range of result: [-32768, 32512].
 *   int16_t __gj_pcm_u8_bias_to_s16  (alias)
 *   __libcgj_batch1673_marker = "libcgj-batch1673"
 *
 * Audio/sample exclusive integer wave. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1673_marker[] = "libcgj-batch1673";

#define B1673_U8_BIAS  128

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_u8_bias_to_s16 — expand biased u8 sample to s16 PCM.
 *
 * Classic WAV 8-bit unsigned mid-point 128 → signed 16-bit, left-shifted
 * 8 so full-scale low-byte samples occupy the high byte of s16.
 */
int16_t
gj_pcm_u8_bias_to_s16(uint8_t u8S)
{
	int32_t i32Centred;

	i32Centred = (int32_t)u8S - B1673_U8_BIAS;
	return (int16_t)(i32Centred << 8);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_u8_bias_to_s16(uint8_t u8S)
    __attribute__((alias("gj_pcm_u8_bias_to_s16")));
