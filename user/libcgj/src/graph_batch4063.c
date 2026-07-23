/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4063: Perlin smootherstep (u32 _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_smootherstep_u(uint32_t edge0, uint32_t edge1,
 *       uint32_t x);
 *     - Ken Perlin smootherstep of x between edge0 and edge1. Returns
 *       unsigned Q16.16 (ONE = 0x00010000): 0 at/before the edge0 side,
 *       ONE at/after the edge1 side, and t^3*(t*(t*6-15)+10) between
 *       (zero 1st and 2nd derivatives at endpoints). Direction is
 *       preserved when edge0 > edge1. Degenerate edges act like step.
 *   uint32_t __gj_u32_smootherstep_u  (alias)
 *   __libcgj_batch4063_marker = "libcgj-batch4063"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique
 * gj_u32_smootherstep_u surface only; no multi-def. Distinct from
 * gj_u32_smoothstep_u (batch4062), gj_u32_smooth_u (batch3459), and
 * gj_smoothstep_q16_u (batch3057). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4063_marker[] = "libcgj-batch4063";

/* Q16.16 unit: 1.0 == 65536. */
#define B4063_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Normalize x into Q16.16 t in [0, ONE] along the directed span
 * edge0 → edge1. Degenerate span returns step-like 0 or ONE.
 */
static uint32_t
b4063_norm_t(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	uint32_t u32SrcLo;
	uint32_t u32SrcHi;
	uint32_t u32Span;
	uint32_t u32Off;
	uint64_t u64Prod;

	if (u32Edge0 == u32Edge1) {
		return (u32X >= u32Edge0) ? B4063_Q16_ONE : 0u;
	}

	if (u32Edge0 < u32Edge1) {
		u32SrcLo = u32Edge0;
		u32SrcHi = u32Edge1;
	} else {
		u32SrcLo = u32Edge1;
		u32SrcHi = u32Edge0;
	}

	if (u32X <= u32SrcLo) {
		return (u32Edge0 < u32Edge1) ? 0u : B4063_Q16_ONE;
	}
	if (u32X >= u32SrcHi) {
		return (u32Edge0 < u32Edge1) ? B4063_Q16_ONE : 0u;
	}

	if (u32Edge1 > u32Edge0) {
		u32Span = u32Edge1 - u32Edge0;
		u32Off = u32X - u32Edge0;
	} else {
		u32Span = u32Edge0 - u32Edge1;
		u32Off = u32Edge0 - u32X;
	}

	u64Prod = (uint64_t)u32Off * (uint64_t)B4063_Q16_ONE;
	return (uint32_t)(u64Prod / (uint64_t)u32Span);
}

/*
 * smootherstep(t) = t^3 * (t*(t*6 - 15) + 10) = 6t^5 - 15t^4 + 10t^3
 * in Q16.16. Uses the expanded poly 6t^2 - 15t + 10 (always >= 1 on
 * [0,1]) so intermediates stay non-negative.
 *
 *   poly = 6*(t*t >> 16) - 15*t + 10*ONE     (Q16)
 *   t3   = (t * t * t) >> 16                 (Q32 scale: t_real^3 * 2^32)
 *   out  = (t3 * poly) >> 32                 (Q16)
 */
static uint32_t
b4063_perlin(uint32_t u32T)
{
	uint64_t u64Tt;
	uint64_t u64T3;
	uint64_t u64Poly;
	uint64_t u64Prod;

	if (u32T == 0u) {
		return 0u;
	}
	if (u32T >= B4063_Q16_ONE) {
		return B4063_Q16_ONE;
	}

	u64Tt = (uint64_t)u32T * (uint64_t)u32T;
	/* t^3 as Q32: (t*t*t) >> 16 */
	u64T3 = (u64Tt * (uint64_t)u32T) >> 16;

	/* poly = 6*t^2 - 15*t + 10 in Q16 (t^2 via >>16). */
	u64Poly = (6ull * (u64Tt >> 16))
	    + (10ull * (uint64_t)B4063_Q16_ONE)
	    - (15ull * (uint64_t)u32T);

	u64Prod = u64T3 * u64Poly;
	return (uint32_t)(u64Prod >> 32);
}

static uint32_t
b4063_smootherstep(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	return b4063_perlin(b4063_norm_t(u32Edge0, u32Edge1, u32X));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_smootherstep_u - Perlin smootherstep on uint32 edges.
 *
 * edge0: start of transition (may be greater than edge1)
 * edge1: end of transition
 * x:     sample value
 *
 * Returns Q16.16 in [0, ONE] with zero first and second derivatives at
 * the endpoints. Self-contained; no parent wires.
 */
uint32_t
gj_u32_smootherstep_u(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	(void)NULL;
	return b4063_smootherstep(u32Edge0, u32Edge1, u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_smootherstep_u(uint32_t u32Edge0, uint32_t u32Edge1,
    uint32_t u32X)
    __attribute__((alias("gj_u32_smootherstep_u")));
