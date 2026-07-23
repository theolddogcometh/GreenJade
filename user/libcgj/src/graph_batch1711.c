/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1711: half-open uint64 range containment.
 *
 * Surface (unique symbols):
 *   int gj_range_u64_contains(uint64_t lo, uint64_t hi, uint64_t x);
 *     — Half-open range [lo, hi). Returns 1 if lo <= x < hi, else 0.
 *       Empty ranges (lo >= hi) never contain.
 *   int __gj_range_u64_contains  (alias)
 *   __libcgj_batch1711_marker = "libcgj-batch1711"
 *
 * Interval / range exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1711_marker[] = "libcgj-batch1711";

/* ---- freestanding helpers ---------------------------------------------- */

/* Non-empty half-open [lo, hi). */
static int
b1711_nonempty(uint64_t u64Lo, uint64_t u64Hi)
{
	return (u64Lo < u64Hi) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_u64_contains — true if x lies in half-open [lo, hi).
 *
 * lo: inclusive lower bound
 * hi: exclusive upper bound
 * x:  query value
 *
 * Returns 1 if contained, 0 otherwise.
 */
int
gj_range_u64_contains(uint64_t u64Lo, uint64_t u64Hi, uint64_t u64X)
{
	if (b1711_nonempty(u64Lo, u64Hi) == 0) {
		return 0;
	}
	if (u64X < u64Lo || u64X >= u64Hi) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_u64_contains(uint64_t u64Lo, uint64_t u64Hi, uint64_t u64X)
    __attribute__((alias("gj_range_u64_contains")));
