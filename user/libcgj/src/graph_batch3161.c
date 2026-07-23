/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3161: half-open uint64 range containment (_u).
 *
 * Surface (unique symbols):
 *   int gj_range_contains_u(uint64_t lo, uint64_t hi, uint64_t x);
 *     - Half-open range [lo, hi). Returns 1 if lo <= x < hi, else 0.
 *       Empty ranges (lo >= hi) never contain.
 *   int __gj_range_contains_u  (alias)
 *   __libcgj_batch3161_marker = "libcgj-batch3161"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_range_u64_contains (batch1711) / gj_range_i64_contains (batch1715)
 * — unique gj_range_contains_u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3161_marker[] = "libcgj-batch3161";

/* ---- freestanding helpers ---------------------------------------------- */

/* Non-empty half-open [lo, hi). */
static int
b3161_nonempty(uint64_t u64Lo, uint64_t u64Hi)
{
	return (u64Lo < u64Hi) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_contains_u - true if x lies in half-open [lo, hi).
 *
 * lo: inclusive lower bound
 * hi: exclusive upper bound
 * x:  query value
 *
 * Returns 1 if contained, 0 otherwise. No parent wires.
 */
int
gj_range_contains_u(uint64_t u64Lo, uint64_t u64Hi, uint64_t u64X)
{
	(void)NULL;
	if (b3161_nonempty(u64Lo, u64Hi) == 0) {
		return 0;
	}
	if (u64X < u64Lo || u64X >= u64Hi) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_contains_u(uint64_t u64Lo, uint64_t u64Hi, uint64_t u64X)
    __attribute__((alias("gj_range_contains_u")));
