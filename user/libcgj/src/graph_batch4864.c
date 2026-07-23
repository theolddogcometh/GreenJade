/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4864: saturating mix of two s16 PCM samples.
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_mix_s16_u(int16_t a, int16_t b);
 *     - Sum a and b in int32, then clamp the sum into
 *       [-32768, 32767]. Saturating additive mix (not average).
 *   int16_t __gj_pcm_mix_s16_u  (alias)
 *   __libcgj_batch4864_marker = "libcgj-batch4864"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_mono_mix_s16 (batch1675) and gj_pcm_clamp_s16_u (batch4863)
 * — unique gj_pcm_mix_s16_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4864_marker[] = "libcgj-batch4864";

#define B4864_S16_MAX  32767
#define B4864_S16_MIN  (-32768)

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b into s16. */
static int16_t
b4864_mix_sat(int16_t i16A, int16_t i16B)
{
	int32_t i32Sum;

	i32Sum = (int32_t)i16A + (int32_t)i16B;
	if (i32Sum > B4864_S16_MAX) {
		return (int16_t)B4864_S16_MAX;
	}
	if (i32Sum < B4864_S16_MIN) {
		return (int16_t)B4864_S16_MIN;
	}
	return (int16_t)i32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_mix_s16_u - saturating additive mix of two s16 samples.
 *
 * a, b: signed PCM samples
 *
 * Returns clamped sum. Self-contained; no parent wires.
 */
int16_t
gj_pcm_mix_s16_u(int16_t i16A, int16_t i16B)
{
	(void)NULL;
	return b4864_mix_sat(i16A, i16B);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_mix_s16_u(int16_t i16A, int16_t i16B)
    __attribute__((alias("gj_pcm_mix_s16_u")));
