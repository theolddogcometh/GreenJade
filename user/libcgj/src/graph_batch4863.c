/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4863: clamp int32 into signed 16-bit PCM.
 *
 * Surface (unique symbols):
 *   int16_t gj_pcm_clamp_s16_u(int32_t x);
 *     - Saturate x into the closed s16 interval [-32768, 32767].
 *       Values above INT16_MAX clamp to 32767; values below INT16_MIN
 *       clamp to -32768; otherwise cast to int16_t.
 *   int16_t __gj_pcm_clamp_s16_u  (alias)
 *   __libcgj_batch4863_marker = "libcgj-batch4863"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870). Distinct from
 * gj_pcm_s16_clip (batch1671) — unique gj_pcm_clamp_s16_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4863_marker[] = "libcgj-batch4863";

#define B4863_S16_MAX  32767
#define B4863_S16_MIN  (-32768)

/* ---- freestanding helpers ---------------------------------------------- */

static int16_t
b4863_clamp_s16(int32_t i32X)
{
	if (i32X > B4863_S16_MAX) {
		return (int16_t)B4863_S16_MAX;
	}
	if (i32X < B4863_S16_MIN) {
		return (int16_t)B4863_S16_MIN;
	}
	return (int16_t)i32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_clamp_s16_u - saturate an int32 accumulator into s16 PCM.
 *
 * x: widened sample (post mix / gain)
 *
 * Returns clamped int16_t. Self-contained; no parent wires.
 */
int16_t
gj_pcm_clamp_s16_u(int32_t i32X)
{
	(void)NULL;
	return b4863_clamp_s16(i32X);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_pcm_clamp_s16_u(int32_t i32X)
    __attribute__((alias("gj_pcm_clamp_s16_u")));
