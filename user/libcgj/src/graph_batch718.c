/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch718: freestanding Bernoulli trial from a
 * uint32_t RNG sample and rational probability p_num / p_den.
 *
 * Surface (unique symbols):
 *   int gj_bernoulli_u32(uint32_t r, uint32_t p_num, uint32_t p_den);
 *     — Return 1 (OK) if r/2^32 < p_num/p_den, else 0 (FAIL).
 *       Approximated without FP via:
 *         (uint64_t)r * (uint64_t)p_den < ((uint64_t)p_num << 32)
 *       p_den == 0 → 0 (FAIL).
 *   int __gj_bernoulli_u32  (alias)
 *   __libcgj_batch718_marker = "libcgj-batch718"
 *
 * Unique gj_ names only; no multi-def with libc / other graph RNG
 * surfaces (e.g. batch212 shuffle LCG, batch106 ChaCha/arc4random).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch718_marker[] = "libcgj-batch718";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bernoulli_u32 — Bernoulli trial: accept when r/2^32 < p_num/p_den.
 *
 * r:     uniform uint32_t sample interpreted as r / 2^32 in [0, 1)
 * p_num: probability numerator
 * p_den: probability denominator (0 → FAIL / return 0)
 *
 * Cross-multiply to avoid floating point and division:
 *   r / 2^32 < p_num / p_den
 *     ⇔  r * p_den < p_num * 2^32
 *     ⇔  (uint64_t)r * (uint64_t)p_den < ((uint64_t)p_num << 32)
 *
 * Both products fit in uint64_t (operands are uint32_t). Returns 1 (OK)
 * on success of the trial, 0 (FAIL) otherwise. Does not set errno.
 */
int
gj_bernoulli_u32(uint32_t r, uint32_t p_num, uint32_t p_den)
{
	uint64_t lhs;
	uint64_t rhs;

	if (p_den == 0u) {
		return 0;
	}

	lhs = (uint64_t)r * (uint64_t)p_den;
	rhs = (uint64_t)p_num << 32;
	return (lhs < rhs) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bernoulli_u32(uint32_t r, uint32_t p_num, uint32_t p_den)
    __attribute__((alias("gj_bernoulli_u32")));
