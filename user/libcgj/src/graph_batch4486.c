/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4486: uint32 linear interpolate with
 * rational weight t/tmax.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lerp_u(uint32_t a, uint32_t b, uint32_t t,
 *                          uint32_t tmax);
 *     - Return a + (b - a) * t / tmax (unsigned, directed).
 *       t==0 → a; t>=tmax (tmax>0) → b; tmax==0 → a.
 *   uint32_t __gj_u32_lerp_u  (alias)
 *   __libcgj_batch4486_marker = "libcgj-batch4486"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_lerp_unorm_u (batch4055)
 * and gj_u32_lerp_q16_u (batch3457) — unique gj_u32_lerp_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4486_marker[] = "libcgj-batch4486";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor((x * t) / tmax) for tmax > 0. */
static uint32_t
b4486_mul_div(uint32_t u32X, uint32_t u32T, uint32_t u32Tmax)
{
	return (uint32_t)(((uint64_t)u32X * (uint64_t)u32T) /
	    (uint64_t)u32Tmax);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lerp_u — unsigned lerp of a,b with rational weight t/tmax.
 *
 * a, b:  endpoints
 * t:     weight numerator (clamped to [0, tmax] when tmax > 0)
 * tmax:  weight denominator (tmax==0 yields a)
 *
 * Formula: a ± floor(|b-a| * t / tmax). Direction uses abs-diff so
 * a>b works without unsigned wrap of (b - a). Self-contained; no
 * parent wires.
 */
uint32_t
gj_u32_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32T, uint32_t u32Tmax)
{
	uint32_t u32Diff;
	uint32_t u32Mix;

	(void)NULL;
	if (u32Tmax == 0u || u32T == 0u) {
		return u32A;
	}
	if (u32T >= u32Tmax) {
		return u32B;
	}
	if (u32A == u32B) {
		return u32A;
	}
	if (u32B >= u32A) {
		u32Diff = u32B - u32A;
		u32Mix = b4486_mul_div(u32Diff, u32T, u32Tmax);
		return u32A + u32Mix;
	}
	u32Diff = u32A - u32B;
	u32Mix = b4486_mul_div(u32Diff, u32T, u32Tmax);
	return u32A - u32Mix;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32T,
    uint32_t u32Tmax)
    __attribute__((alias("gj_u32_lerp_u")));
