/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4865: mono sample from stereo L/R pair.
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_mono_from_stereo_u(int16_t l, int16_t r);
 *     - Average left and right s16 channels into one mono sample:
 *       ((int32_t)l + (int32_t)r) / 2 with truncating division toward
 *       zero. Sum of two full-scale s16 values always fits int32; the
 *       average always fits int16 (no external clip).
 *   int16_t __gj_pcm_mono_from_stereo_u  (alias)
 *   __libcgj_batch4865_marker = "libcgj-batch4865"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_mono_mix_s16 (batch1675) and gj_pcm_mix_s16_u (batch4864) —
 * unique gj_pcm_mono_from_stereo_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4865_marker[] = "libcgj-batch4865";

/* ---- freestanding helpers ---------------------------------------------- */

static int16_t
b4865_mono(int16_t i16L, int16_t i16R)
{
	int32_t i32Sum;

	i32Sum = (int32_t)i16L + (int32_t)i16R;
	return (int16_t)(i32Sum / 2);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_mono_from_stereo_u - stereo L/R → mono average.
 *
 * l: left channel sample
 * r: right channel sample
 *
 * Returns mono int16_t. Self-contained; no parent wires.
 */
int16_t
gj_pcm_mono_from_stereo_u(int16_t i16L, int16_t i16R)
{
	(void)NULL;
	return b4865_mono(i16L, i16R);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_mono_from_stereo_u(int16_t i16L, int16_t i16R)
    __attribute__((alias("gj_pcm_mono_from_stereo_u")));
