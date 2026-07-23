/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3459: Q16.16 Hermite smoothstep (u32 _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_smooth_u(uint32_t x);
 *     - Hermite smoothstep on an unsigned Q16.16 unit interval value:
 *       clamp x to [0, 1.0], then t*t*(3 - 2*t). ONE = 0x00010000.
 *   uint32_t __gj_u32_smooth_u  (alias)
 *   __libcgj_batch3459_marker = "libcgj-batch3459"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u32_smooth_u surface only; no multi-def. Distinct from
 * gj_smoothstep_q16_u (batch3057). No parent wires (clamp inlined).
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3459_marker[] = "libcgj-batch3459";

/* Q16.16 unit: 1.0 == 65536. */
#define B3459_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Clamp x into [0, ONE] for unsigned Q16.16 (lower bound is always 0).
 */
static uint32_t
b3459_clamp01(uint32_t u32X)
{
	if (u32X > B3459_Q16_ONE) {
		return B3459_Q16_ONE;
	}
	return u32X;
}

/*
 * smoothstep(t) = t^2 * (3 - 2*t) in Q16.16.
 * With t in [0, ONE]: result = (t * t * (3*ONE - 2*t)) >> 32.
 */
static uint32_t
b3459_smooth(uint32_t u32X)
{
	uint32_t u32T;
	uint64_t u64Tt;
	uint64_t u64S;
	uint64_t u64Prod;

	u32T = b3459_clamp01(u32X);
	u64Tt = (uint64_t)u32T * (uint64_t)u32T;
	/* 3 - 2*t in Q16: 3*ONE - 2*t */
	u64S = (3ull * (uint64_t)B3459_Q16_ONE) - (2ull * (uint64_t)u32T);
	u64Prod = u64Tt * u64S;
	return (uint32_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_smooth_u - Hermite smoothstep on unsigned Q16.16.
 *
 * x: unit-interval Q16.16 value (values outside [0, 1.0] are clamped)
 *
 * Returns smoothstep(x) in the same Q16.16 scale. Endpoints: 0 → 0,
 * ONE → ONE. Self-contained; no parent wires to clamp01.
 * Does not call libc.
 */
uint32_t
gj_u32_smooth_u(uint32_t u32X)
{
	(void)NULL;
	return b3459_smooth(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_smooth_u(uint32_t u32X)
    __attribute__((alias("gj_u32_smooth_u")));
