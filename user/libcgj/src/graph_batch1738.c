/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1738: clamp uint64_t into int64_t range.
 *
 * Surface (unique symbols):
 *   int64_t gj_u64_clamped_to_i64(uint64_t x);
 *     — If x <= (uint64_t)INT64_MAX, return (int64_t)x; else INT64_MAX.
 *       Never produces a negative result (unsigned input domain).
 *   int64_t __gj_u64_clamped_to_i64  (alias)
 *   __libcgj_batch1738_marker = "libcgj-batch1738"
 *
 * Decimal/fixed-print exclusive (safe downcast for signed fixed fields).
 * Distinct from gj_u64_clamp_range / gj_u64_clamped_inc. Unique; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1738_marker[] = "libcgj-batch1738";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1738_i64_max_u — INT64_MAX as uint64_t for comparison.
 */
static uint64_t
b1738_i64_max_u(void)
{
	return (uint64_t)INT64_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clamped_to_i64 — map u64 into [0, INT64_MAX] as int64_t.
 */
int64_t
gj_u64_clamped_to_i64(uint64_t u64X)
{
	if (u64X > b1738_i64_max_u()) {
		return INT64_MAX;
	}
	return (int64_t)u64X;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_u64_clamped_to_i64(uint64_t u64X)
    __attribute__((alias("gj_u64_clamped_to_i64")));
