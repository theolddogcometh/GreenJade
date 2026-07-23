/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2189: reservoir Algorithm-R accept predicate.
 *
 * Surface (unique symbols):
 *   int gj_reservoir_accept_p(uint32_t r, size_t k, size_t t);
 *     - Return 1 if a reservoir of size k should replace a random slot
 *       with incoming index t, given uniform sample r on [0, 2^32).
 *       Accepts when uniform index j = mulhi(r, t+1) satisfies j < k
 *       (probability k/(t+1) for t+1 in 1..2^32). Defensive:
 *         k == 0 -> 0; t+1 overflow / empty range -> 0.
 *   int __gj_reservoir_accept_p  (alias)
 *   __libcgj_batch2189_marker = "libcgj-batch2189"
 *
 * Distinct from gj_reservoir_u32 (batch304 full Algorithm R sampler).
 * Unique accept_p predicate for the post-2180 random wave; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2189_marker[] = "libcgj-batch2189";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map r into [0, bound) via multiply-high (Lemire-style, single draw).
 * bound == 0 returns 0. bound > UINT32_MAX is treated as full-range
 * (return r) when size_t is wider than 32 bits.
 */
static size_t
b2189_index(uint32_t u32R, size_t cBound)
{
	uint64_t u64Prod;

	if (cBound <= 1u) {
		return 0u;
	}
	if (cBound > (size_t)UINT32_MAX) {
		return (size_t)u32R;
	}
	u64Prod = (uint64_t)u32R * (uint64_t)cBound;
	return (size_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reservoir_accept_p - true if Algorithm R would replace a slot.
 *
 * r: uniform uint32 sample for this step
 * k: reservoir capacity (number of kept indices)
 * t: current stream index (0-based); population seen so far is t+1
 *
 * Classic Algorithm R condition for t >= k:
 *   j = uniform in [0, t]  (bound = t+1)
 *   accept replacement when j < k
 *
 * When t+1 would wrap size_t, refuse (return 0). When k == 0, refuse.
 * When k > t+1, every draw accepts (j always < k for bound = t+1).
 */
int
gj_reservoir_accept_p(uint32_t u32R, size_t cK, size_t cT)
{
	size_t cBound;
	size_t iJ;

	(void)NULL;

	if (cK == 0u) {
		return 0;
	}

	/* bound = t + 1; refuse on wrap. */
	cBound = cT + 1u;
	if (cBound == 0u) {
		return 0;
	}

	iJ = b2189_index(u32R, cBound);
	return (iJ < cK) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_reservoir_accept_p(uint32_t u32R, size_t cK, size_t cT)
    __attribute__((alias("gj_reservoir_accept_p")));
