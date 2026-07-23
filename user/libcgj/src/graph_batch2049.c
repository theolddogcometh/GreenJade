/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2049: int64_t closed-interval membership predicate.
 *
 * Surface (unique symbols):
 *   int gj_i64_in_range_p(int64_t x, int64_t lo, int64_t hi);
 *     - Return 1 if lo <= x <= hi after normalizing bounds: if lo > hi
 *       the bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)]. Inclusive on both ends. Else 0.
 *   int __gj_i64_in_range_p  (alias)
 *   __libcgj_batch2049_marker = "libcgj-batch2049"
 *
 * Post-2000 i64 arith exclusive wave (2041-2050). Distinct from
 * gj_u64_in_range (batch737) - unique gj_i64_in_range_p surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2049_marker[] = "libcgj-batch2049";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2049_in_range - 1 if x is in the closed interval [lo, hi] after
 * normalizing inverted bounds; else 0.
 */
static int
b2049_in_range(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
{
	int64_t i64Tmp;

	if (i64Lo > i64Hi) {
		i64Tmp = i64Lo;
		i64Lo = i64Hi;
		i64Hi = i64Tmp;
	}

	if (i64X < i64Lo) {
		return 0;
	}
	if (i64X > i64Hi) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_in_range_p - 1 if x is in the closed interval [lo, hi], else 0.
 *
 * x:  value under test
 * lo: lower bound (may be greater than hi; swapped conceptually)
 * hi: upper bound
 *
 * Inclusive membership after any needed lo/hi reorder.
 */
int
gj_i64_in_range_p(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
{
	(void)NULL;
	return b2049_in_range(i64X, i64Lo, i64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i64_in_range_p(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
    __attribute__((alias("gj_i64_in_range_p")));
