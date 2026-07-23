/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2187: unbiased-leaning bounded uniform map.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uniform_u32_bounded(uint32_t r, uint32_t bound);
 *     - Map a full-range uniform draw r into [0, bound) via Lemire-style
 *       multiply-high. When low32(r*bound) is in the classic reject
 *       region, scramble r once and re-map so a single-draw API stays
 *       in range without a second external PRNG call. bound == 0 -> 0.
 *   uint32_t __gj_uniform_u32_bounded  (alias)
 *   __libcgj_batch2187_marker = "libcgj-batch2187"
 *
 * Distinct from gj_uniform_u32 (batch717 plain multiply-high) and from
 * arc4random_uniform (batch106 multi-draw reject). Unique _bounded
 * export for the post-2180 random wave; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2187_marker[] = "libcgj-batch2187";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Lemire 32-bit product: returns high 32 of (r * bound) as the candidate
 * index, and leaves the low 32 in *pLow for threshold checks.
 */
static uint32_t
b2187_mulhi(uint32_t u32R, uint32_t u32Bound, uint32_t *pLow)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32R * (uint64_t)u32Bound;
	*pLow = (uint32_t)u64Prod;
	return (uint32_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uniform_u32_bounded - map r into [0, bound) with Lemire threshold.
 *
 * r:     uniform integer on the full uint32 domain [0, 2^32)
 * bound: exclusive upper bound of the result range
 *
 * Computes m = r * bound (64-bit). Candidate = high32(m).
 * threshold t = (0 - bound) mod 2^32.
 * When low32(m) >= t the candidate is accepted as-is (unbiased region).
 * When low32(m) < t (classic Lemire reject zone for multi-draw APIs),
 * re-mix r once via a cheap xorshift-style scramble and take mulhi of
 * the remixed sample so the single-draw API still returns in range
 * without requiring a second external PRNG call.
 *
 * bound == 0 returns 0 without multiplying. bound == 1 returns 0.
 */
uint32_t
gj_uniform_u32_bounded(uint32_t u32R, uint32_t u32Bound)
{
	uint32_t u32Low;
	uint32_t u32Hi;
	uint32_t u32Thresh;
	uint32_t u32Alt;

	(void)NULL;

	if (u32Bound <= 1u) {
		return 0u;
	}

	u32Hi = b2187_mulhi(u32R, u32Bound, &u32Low);
	u32Thresh = 0u - u32Bound;

	if (u32Low >= u32Thresh) {
		return u32Hi;
	}

	/* Single-draw fallback: scramble r and re-map (still in range). */
	u32Alt = u32R ^ (u32R << 13);
	u32Alt ^= u32Alt >> 17;
	u32Alt ^= u32Alt << 5;
	if (u32Alt == 0u) {
		u32Alt = u32R | 1u;
	}
	return b2187_mulhi(u32Alt, u32Bound, &u32Low);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uniform_u32_bounded(uint32_t u32R, uint32_t u32Bound)
    __attribute__((alias("gj_uniform_u32_bounded")));
