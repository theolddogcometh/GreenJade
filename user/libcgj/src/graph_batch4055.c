/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4055: uint32_t lerp with unorm weight
 * (exclusive lerp_unorm_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lerp_unorm_u(uint32_t a, uint32_t b, uint32_t t);
 *     - Linear interpolate a→b with t as unorm32 in [0, UINT32_MAX]
 *       mapped to [0.0, 1.0]. t==0 → a; t==UINT32_MAX → b.
 *       Formula: a ± floor(|b-a| * t / UINT32_MAX).
 *   uint32_t __gj_u32_lerp_unorm_u  (alias)
 *   __libcgj_batch4055_marker = "libcgj-batch4055"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u32_lerp_unorm_u surface only; no multi-def. Distinct from
 * gj_u32_lerp_q16 (batch1478), gj_u32_lerp_q16_u (batch3457).
 * No parent wires. No __int128 (uses uint64 product).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4055_marker[] = "libcgj-batch4055";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor of (x * t) / UINT32_MAX for full-range unorm t. */
static uint32_t
b4055_mul_div_max(uint32_t u32X, uint32_t u32T)
{
	return (uint32_t)(((uint64_t)u32X * (uint64_t)u32T) /
	    (uint64_t)UINT32_MAX);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lerp_unorm_u — unsigned lerp of a,b with unorm32 weight t.
 *
 * t==0 returns a; t==UINT32_MAX returns b. Direction uses abs-diff so
 * a>b works without unsigned wrap of (b - a). Mix scaled in uint64_t.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_lerp_unorm_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	uint32_t u32Diff;
	uint32_t u32Mix;

	(void)NULL;
	if (u32T == 0u) {
		return u32A;
	}
	if (u32T == UINT32_MAX) {
		return u32B;
	}
	if (u32A == u32B) {
		return u32A;
	}
	if (u32B >= u32A) {
		u32Diff = u32B - u32A;
		u32Mix = b4055_mul_div_max(u32Diff, u32T);
		return u32A + u32Mix;
	}
	u32Diff = u32A - u32B;
	u32Mix = b4055_mul_div_max(u32Diff, u32T);
	return u32A - u32Mix;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lerp_unorm_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
    __attribute__((alias("gj_u32_lerp_unorm_u")));
