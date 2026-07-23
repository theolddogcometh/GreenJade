/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3264: uint32_t closed-interval membership (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_in_range_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Return 1 if x is in the closed interval [lo, hi] after
 *       normalizing bounds: if lo > hi the bounds are swapped so the
 *       effective range is always [min(lo,hi), max(lo,hi)]. Inclusive
 *       on both ends.
 *   int __gj_u32_in_range_u  (alias)
 *   __libcgj_batch3264_marker = "libcgj-batch3264"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u32_in_range_u surface only; no multi-def. Distinct from
 * gj_u64_in_range (batch737). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3264_marker[] = "libcgj-batch3264";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if x is inside the closed interval between lo and hi (order-free).
 */
static int
b3264_in_range(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	uint32_t u32Tmp;

	if (u32Lo > u32Hi) {
		u32Tmp = u32Lo;
		u32Lo = u32Hi;
		u32Hi = u32Tmp;
	}

	if (u32X < u32Lo) {
		return 0;
	}
	if (u32X > u32Hi) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_in_range_u - 1 if x is in the closed interval [lo, hi], else 0.
 *
 * x:  value under test
 * lo: lower bound (may be greater than hi; swapped conceptually)
 * hi: upper bound
 *
 * Inclusive membership after reordering bounds. No parent wires.
 */
int
gj_u32_in_range_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b3264_in_range(u32X, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_in_range_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_u32_in_range_u")));
