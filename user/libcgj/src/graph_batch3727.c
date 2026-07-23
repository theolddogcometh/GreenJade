/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3727: freestanding map u32 draw into [lo, hi)
 * (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rng_range_u32_u(uint32_t r, uint32_t lo, uint32_t hi);
 *     - Map a full-range uniform draw r on [0, 2^32) into the half-open
 *       interval [lo, hi) via multiply-high of (hi - lo). Empty or
 *       inverted range (hi <= lo) returns lo without multiplying.
 *   uint32_t __gj_rng_range_u32_u  (alias)
 *   __libcgj_batch3727_marker = "libcgj-batch3727"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_rng_uniform_u32_u (batch3725, bound-from-zero form) — unique
 * range surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3727_marker[] = "libcgj-batch3727";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map r into [0, bound) via 32x32->64 multiply-high.
 * bound must be non-zero on entry.
 */
static uint32_t
b3727_map(uint32_t u32R, uint32_t u32Bound)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32R * (uint64_t)u32Bound;
	return (uint32_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_range_u32_u — map r into half-open [lo, hi).
 *
 * r:  uniform integer on the full uint32 domain [0, 2^32)
 * lo: inclusive lower bound of the result range
 * hi: exclusive upper bound of the result range
 *
 * Computes lo + high_32(r * (hi - lo)). When hi <= lo the span is empty
 * or inverted and the function returns lo (defensive clamp).
 */
uint32_t
gj_rng_range_u32_u(uint32_t r, uint32_t lo, uint32_t hi)
{
	uint32_t u32Span;

	(void)NULL;

	if (hi <= lo) {
		return lo;
	}

	u32Span = hi - lo;
	return lo + b3727_map(r, u32Span);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rng_range_u32_u(uint32_t r, uint32_t lo, uint32_t hi)
    __attribute__((alias("gj_rng_range_u32_u")));
