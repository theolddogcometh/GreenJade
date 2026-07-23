/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2045: saturating int64_t absolute value (sat_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_abs_sat_u(int64_t x);
 *     - Returns |x| as int64_t. INT64_MIN saturates to INT64_MAX
 *       (no UB on two's-complement negation of the most-negative value).
 *   int64_t __gj_i64_abs_sat_u  (alias)
 *   __libcgj_batch2045_marker = "libcgj-batch2045"
 *
 * Post-2000 i64 arith exclusive wave (2041-2050). Distinct from
 * gj_i64_abs_u (batch613) which returns uint64_t, and from gj_abs_i64
 * (batch378) - unique gj_i64_abs_sat_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2045_marker[] = "libcgj-batch2045";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2045_is_min - true iff x is INT64_MIN (abs not representable in int64_t).
 */
static int
b2045_is_min(int64_t i64X)
{
	return (i64X == INT64_MIN) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_abs_sat_u - |x| with saturation at INT64_MAX for INT64_MIN.
 */
int64_t
gj_i64_abs_sat_u(int64_t i64X)
{
	(void)NULL;
	if (b2045_is_min(i64X) != 0) {
		return INT64_MAX;
	}
	if (i64X < 0) {
		return -i64X;
	}
	return i64X;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_abs_sat_u(int64_t i64X)
    __attribute__((alias("gj_i64_abs_sat_u")));
