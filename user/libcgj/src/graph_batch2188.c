/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2188: Bernoulli trial with milli-probability.
 *
 * Surface (unique symbols):
 *   int gj_bernoulli_milli_p(uint32_t r, uint32_t milli_p);
 *     - Return 1 if the uniform sample r/2^32 is strictly less than
 *       milli_p / 1000, else 0. milli_p is a probability in parts per
 *       thousand (0..1000). milli_p == 0 -> always 0; milli_p >= 1000
 *       -> always 1. No floating point.
 *   int __gj_bernoulli_milli_p  (alias)
 *   __libcgj_batch2188_marker = "libcgj-batch2188"
 *
 * Distinct from gj_bernoulli_u32 (batch718 rational p_num/p_den). Unique
 * milli_p export for the post-2180 random wave; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2188_marker[] = "libcgj-batch2188";

/* Parts-per-thousand denominator for milli_p. */
#define B2188_MILLI  1000u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Compare r/2^32 < milli_p / 1000 without FP or division of r:
 *   r * 1000 < milli_p * 2^32
 * Left product fits in uint64 (r and 1000 are 32-bit scale).
 * Right product uses uint64 milli_p << 32 (milli_p <= 1000).
 */
static int
b2188_trial(uint32_t u32R, uint32_t u32Milli)
{
	uint64_t u64Lhs;
	uint64_t u64Rhs;

	u64Lhs = (uint64_t)u32R * (uint64_t)B2188_MILLI;
	u64Rhs = (uint64_t)u32Milli << 32;
	return (u64Lhs < u64Rhs) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bernoulli_milli_p - Bernoulli trial with p = milli_p / 1000.
 *
 * r:       uniform uint32_t sample interpreted as r / 2^32 in [0, 1)
 * milli_p: probability in parts per thousand (0 = never, 1000 = always)
 *
 * Returns 1 (accept) or 0 (reject). Does not set errno.
 */
int
gj_bernoulli_milli_p(uint32_t u32R, uint32_t u32MilliP)
{
	(void)NULL;

	if (u32MilliP == 0u) {
		return 0;
	}
	if (u32MilliP >= B2188_MILLI) {
		return 1;
	}

	return b2188_trial(u32R, u32MilliP);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bernoulli_milli_p(uint32_t u32R, uint32_t u32MilliP)
    __attribute__((alias("gj_bernoulli_milli_p")));
