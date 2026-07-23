/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1737: saturating int64_t negation.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_neg_sat(int64_t x);
 *     — Returns -x. INT64_MIN saturates to INT64_MAX (no UB on
 *       two's-complement negation of the most-negative value).
 *   int64_t __gj_i64_neg_sat  (alias)
 *   __libcgj_batch1737_marker = "libcgj-batch1737"
 *
 * Decimal/fixed-print exclusive (safe magnitude for signed emit).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1737_marker[] = "libcgj-batch1737";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1737_is_min — true iff x is INT64_MIN (cannot negate safely).
 */
static int
b1737_is_min(int64_t i64X)
{
	return (i64X == INT64_MIN) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_neg_sat — -x with saturation at INT64_MAX for INT64_MIN.
 */
int64_t
gj_i64_neg_sat(int64_t i64X)
{
	if (b1737_is_min(i64X) != 0) {
		return INT64_MAX;
	}
	return -i64X;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_neg_sat(int64_t i64X)
    __attribute__((alias("gj_i64_neg_sat")));
