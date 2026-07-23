/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1739: clamp int64_t into uint64_t range.
 *
 * Surface (unique symbols):
 *   uint64_t gj_i64_clamped_to_u64(int64_t x);
 *     — Negative x clamps to 0. Non-negative x returns (uint64_t)x
 *       (always in range for signed non-negative values).
 *   uint64_t __gj_i64_clamped_to_u64  (alias)
 *   __libcgj_batch1739_marker = "libcgj-batch1739"
 *
 * Decimal/fixed-print exclusive (unsigned field width / digit loops).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1739_marker[] = "libcgj-batch1739";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1739_is_neg — true iff x < 0.
 */
static int
b1739_is_neg(int64_t i64X)
{
	return (i64X < 0) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_clamped_to_u64 — map i64 into [0, INT64_MAX] as uint64_t.
 *
 * Negatives become 0; non-negatives cast without change of magnitude.
 */
uint64_t
gj_i64_clamped_to_u64(int64_t i64X)
{
	if (b1739_is_neg(i64X) != 0) {
		return 0ull;
	}
	return (uint64_t)i64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_i64_clamped_to_u64(int64_t i64X)
    __attribute__((alias("gj_i64_clamped_to_u64")));
