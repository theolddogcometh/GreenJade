/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4532: uint64_t closed-interval membership (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_in_range_u2(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Return 1 if x is in the closed interval [lo, hi] after
 *       normalizing bounds: if lo > hi the bounds are swapped so the
 *       effective range is always [min(lo,hi), max(lo,hi)]. Inclusive
 *       on both ends.
 *   int __gj_u64_in_range_u2  (alias)
 *   __libcgj_batch4532_marker = "libcgj-batch4532"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_u64_in_range_u2 surface only; no multi-def. Distinct from
 * gj_u64_in_range (batch737) and gj_u32_in_range_u (batch4531). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4532_marker[] = "libcgj-batch4532";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if x is inside the closed interval between lo and hi (order-free).
 */
static int
b4532_in_range(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Tmp;

	if (u64Lo > u64Hi) {
		u64Tmp = u64Lo;
		u64Lo = u64Hi;
		u64Hi = u64Tmp;
	}

	if (u64X < u64Lo) {
		return 0;
	}
	if (u64X > u64Hi) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_in_range_u2 - 1 if x is in the closed interval [lo, hi], else 0.
 *
 * x:  value under test
 * lo: lower bound (may be greater than hi; swapped conceptually)
 * hi: upper bound
 *
 * Inclusive membership after reordering bounds. Exclusive _u surface
 * distinct from gj_u64_in_range. No parent wires.
 */
int
gj_u64_in_range_u2(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b4532_in_range(u64X, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_in_range_u2(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_u64_in_range_u2")));
