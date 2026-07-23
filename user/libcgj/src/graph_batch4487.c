/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4487: uint32 inverse lerp to unorm with
 * clamp (rational wave; unique surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_inv_lerp_u2(uint32_t a, uint32_t b, uint32_t x);
 *     - Inverse of unorm-style lerp: unorm t such that lerp(a,b,t) ≈ x.
 *       Returns floor(|x-a| * UINT32_MAX / |b-a|), clamped to
 *       [0, UINT32_MAX]. a==b → 0.
 *   uint32_t __gj_u32_inv_lerp_u2  (alias)
 *   __libcgj_batch4487_marker = "libcgj-batch4487"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_inv_lerp_u (batch4057) —
 * unique _u2 surface only; no multi-def. Companion of gj_u32_lerp_u
 * (batch4486) but self-contained (no parent wires). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4487_marker[] = "libcgj-batch4487";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor of (num * UINT32_MAX) / den for den > 0. */
static uint32_t
b4487_scale_to_unorm(uint32_t u32Num, uint32_t u32Den)
{
	return (uint32_t)(((uint64_t)u32Num * (uint64_t)UINT32_MAX) /
	    (uint64_t)u32Den);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_inv_lerp_u2 — unorm weight of x between a and b (clamped).
 *
 * a, b: range endpoints (order may be inverted)
 * x:    sample value
 *
 * When a < b:
 *   x <= a → 0; x >= b → UINT32_MAX; else floor((x-a)*MAX/(b-a)).
 * When a > b:
 *   x >= a → 0; x <= b → UINT32_MAX; else floor((a-x)*MAX/(a-b)).
 * When a == b: returns 0. Self-contained; no parent wires.
 */
uint32_t
gj_u32_inv_lerp_u2(uint32_t u32A, uint32_t u32B, uint32_t u32X)
{
	uint32_t u32Span;
	uint32_t u32Off;

	(void)NULL;
	if (u32A == u32B) {
		return 0u;
	}
	if (u32B > u32A) {
		if (u32X <= u32A) {
			return 0u;
		}
		if (u32X >= u32B) {
			return UINT32_MAX;
		}
		u32Span = u32B - u32A;
		u32Off = u32X - u32A;
		return b4487_scale_to_unorm(u32Off, u32Span);
	}
	/* a > b: inverted range; t=0 at a, t=1 at b. */
	if (u32X >= u32A) {
		return 0u;
	}
	if (u32X <= u32B) {
		return UINT32_MAX;
	}
	u32Span = u32A - u32B;
	u32Off = u32A - u32X;
	return b4487_scale_to_unorm(u32Off, u32Span);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_inv_lerp_u2(uint32_t u32A, uint32_t u32B, uint32_t u32X)
    __attribute__((alias("gj_u32_inv_lerp_u2")));
