/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch737: uint64_t closed-interval membership.
 *
 * Surface (unique symbols):
 *   int gj_u64_in_range(uint64_t x, uint64_t lo, uint64_t hi);
 *     — Return 1 if lo <= x <= hi after normalizing bounds: if lo > hi
 *       the bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)]. Inclusive on both ends.
 *   int __gj_u64_in_range  (alias)
 *   __libcgj_batch737_marker = "libcgj-batch737"
 *
 * Distinct from gj_clamp_u64 (batch587) which returns a clamped value,
 * and from IPv4/IPv6 range helpers. Unique gj_u64_in_range surface —
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch737_marker[] = "libcgj-batch737";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_in_range — 1 if x is in the closed interval [lo, hi], else 0.
 *
 * x:  value under test
 * lo: lower bound (may be greater than hi; swapped conceptually)
 * hi: upper bound
 *
 * If lo > hi, bounds are swapped so the interval is never inverted.
 * Inclusive membership: lo <= x <= hi (after reorder).
 */
int
gj_u64_in_range(uint64_t x, uint64_t lo, uint64_t hi)
{
	uint64_t uTmp;

	if (lo > hi) {
		uTmp = lo;
		lo = hi;
		hi = uTmp;
	}

	if (x < lo) {
		return 0;
	}
	if (x > hi) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_in_range(uint64_t x, uint64_t lo, uint64_t hi)
    __attribute__((alias("gj_u64_in_range")));
