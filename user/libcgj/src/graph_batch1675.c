/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1675: mono downmix of two s16 samples.
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_mono_mix_s16(int16_t a, int16_t b);
 *     — Average two s16 channels into one mono sample:
 *       (int32_t)a + (int32_t)b, arithmetic / 2 (toward −∞ for odds
 *       via signed right shift is NOT used; uses truncating / 2 toward
 *       zero). Result always fits int16; no external clip needed.
 *   int16_t __gj_pcm_mono_mix_s16  (alias)
 *   __libcgj_batch1675_marker = "libcgj-batch1675"
 *
 * Audio/sample exclusive integer wave. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1675_marker[] = "libcgj-batch1675";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_mono_mix_s16 — stereo L/R (or any pair) → mono average.
 *
 * Widens to int32 before divide so the sum of two full-scale s16 values
 * cannot overflow. Truncating integer division toward zero.
 */
int16_t
gj_pcm_mono_mix_s16(int16_t i16A, int16_t i16B)
{
	int32_t i32Sum;

	i32Sum = (int32_t)i16A + (int32_t)i16B;
	return (int16_t)(i32Sum / 2);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_mono_mix_s16(int16_t i16A, int16_t i16B)
    __attribute__((alias("gj_pcm_mono_mix_s16")));
