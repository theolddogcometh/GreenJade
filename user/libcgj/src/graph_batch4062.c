/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4062: GLSL-style Hermite smoothstep (u32 _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_smoothstep_u(uint32_t edge0, uint32_t edge1, uint32_t x);
 *     - Hermite smoothstep of x between edge0 and edge1. Returns an
 *       unsigned Q16.16 unit value (ONE = 0x00010000): 0 at/before the
 *       edge0 side, ONE at/after the edge1 side, and t^2*(3-2*t) between.
 *       Direction is preserved when edge0 > edge1 (result falls 1→0).
 *       Degenerate edges (edge0 == edge1) act like step: ONE if x >= edge0
 *       else 0.
 *   uint32_t __gj_u32_smoothstep_u  (alias)
 *   __libcgj_batch4062_marker = "libcgj-batch4062"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique
 * gj_u32_smoothstep_u surface only; no multi-def. Distinct from
 * gj_smoothstep_q16_u (batch3057) and gj_u32_smooth_u (batch3459).
 * No parent wires (clamp / normalize inlined). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4062_marker[] = "libcgj-batch4062";

/* Q16.16 unit: 1.0 == 65536. */
#define B4062_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Normalize x into Q16.16 t in [0, ONE] along the directed span
 * edge0 → edge1. Degenerate span returns step-like 0 or ONE.
 */
static uint32_t
b4062_norm_t(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	uint32_t u32SrcLo;
	uint32_t u32SrcHi;
	uint32_t u32Span;
	uint32_t u32Off;
	uint64_t u64Prod;

	if (u32Edge0 == u32Edge1) {
		return (u32X >= u32Edge0) ? B4062_Q16_ONE : 0u;
	}

	if (u32Edge0 < u32Edge1) {
		u32SrcLo = u32Edge0;
		u32SrcHi = u32Edge1;
	} else {
		u32SrcLo = u32Edge1;
		u32SrcHi = u32Edge0;
	}

	if (u32X <= u32SrcLo) {
		/* Before low end of ordered span. */
		return (u32Edge0 < u32Edge1) ? 0u : B4062_Q16_ONE;
	}
	if (u32X >= u32SrcHi) {
		return (u32Edge0 < u32Edge1) ? B4062_Q16_ONE : 0u;
	}

	/* Interior: directed offset from edge0 toward edge1. */
	if (u32Edge1 > u32Edge0) {
		u32Span = u32Edge1 - u32Edge0;
		u32Off = u32X - u32Edge0;
	} else {
		u32Span = u32Edge0 - u32Edge1;
		u32Off = u32Edge0 - u32X;
	}

	u64Prod = (uint64_t)u32Off * (uint64_t)B4062_Q16_ONE;
	return (uint32_t)(u64Prod / (uint64_t)u32Span);
}

/*
 * smoothstep(t) = t^2 * (3 - 2*t) in Q16.16.
 * With t in [0, ONE]: result = (t * t * (3*ONE - 2*t)) >> 32.
 */
static uint32_t
b4062_hermite(uint32_t u32T)
{
	uint64_t u64Tt;
	uint64_t u64S;
	uint64_t u64Prod;

	if (u32T == 0u) {
		return 0u;
	}
	if (u32T >= B4062_Q16_ONE) {
		return B4062_Q16_ONE;
	}
	u64Tt = (uint64_t)u32T * (uint64_t)u32T;
	u64S = (3ull * (uint64_t)B4062_Q16_ONE) - (2ull * (uint64_t)u32T);
	u64Prod = u64Tt * u64S;
	return (uint32_t)(u64Prod >> 32);
}

static uint32_t
b4062_smoothstep(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	return b4062_hermite(b4062_norm_t(u32Edge0, u32Edge1, u32X));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_smoothstep_u - GLSL-style Hermite smoothstep on uint32 edges.
 *
 * edge0: start of transition (may be greater than edge1)
 * edge1: end of transition
 * x:     sample value
 *
 * Returns Q16.16 in [0, ONE]. Endpoints map to 0 / ONE; interior uses
 * t*t*(3-2*t). Self-contained; no parent wires.
 */
uint32_t
gj_u32_smoothstep_u(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	(void)NULL;
	return b4062_smoothstep(u32Edge0, u32Edge1, u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_smoothstep_u(uint32_t u32Edge0, uint32_t u32Edge1,
    uint32_t u32X)
    __attribute__((alias("gj_u32_smoothstep_u")));
