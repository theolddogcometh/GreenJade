/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5086: debug closed-range membership (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_range_ok_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Return 1 if x is in the closed interval [lo, hi] with lo <= hi
 *       required as given; if lo > hi return 0 (assert-style, no swap).
 *   uint32_t __gj_dbg_range_ok_u  (alias)
 *   __libcgj_batch5086_marker = "libcgj-batch5086"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_range_ok_u surface only; no multi-def. Distinct from
 * gj_u32_in_range_u2 (batch4531, order-free swap). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5086_marker[] = "libcgj-batch5086";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if lo <= hi and x is inside the closed interval [lo, hi], else 0.
 * Assert-style: inverted bounds are not normalized.
 */
static uint32_t
b5086_range_ok(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	if (u32Lo > u32Hi) {
		return 0u;
	}
	if (u32X < u32Lo) {
		return 0u;
	}
	if (u32X > u32Hi) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_range_ok_u - 1 if x is in closed [lo, hi] with lo <= hi, else 0.
 *
 * x:  value under test
 * lo: lower bound (must be <= hi)
 * hi: upper bound
 *
 * Inclusive membership. Inverted bounds (lo > hi) → 0. No parent wires.
 */
uint32_t
gj_dbg_range_ok_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b5086_range_ok(u32X, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_range_ok_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_dbg_range_ok_u")));
