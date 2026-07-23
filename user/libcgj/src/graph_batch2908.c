/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2908: saturating int64_t subtract (_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_sat_sub_u(int64_t a, int64_t b);
 *     - a - b with saturation at INT64_MAX / INT64_MIN (no wrap).
 *   int64_t __gj_i64_sat_sub_u  (alias)
 *   __libcgj_batch2908_marker = "libcgj-batch2908"
 *
 * Continuum sat-arith exclusive wave (2901-2910). Distinct from
 * gj_i64_sat_sub (batch926) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2908_marker[] = "libcgj-batch2908";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a - b in the int64_t domain. Range tests ensure the sub
 * itself is only performed when the mathematical difference is
 * representable (no signed overflow UB).
 */
static int64_t
b2908_sub_sat(int64_t i64A, int64_t i64B)
{
	if (i64B > 0) {
		if (i64A < (INT64_MIN + i64B)) {
			return INT64_MIN;
		}
	} else if (i64B < 0) {
		if (i64A > (INT64_MAX + i64B)) {
			return INT64_MAX;
		}
	}
	return i64A - i64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_sat_sub_u - signed 64-bit subtract; clamp to INT64_MAX/MIN.
 *
 * a: minuend
 * b: subtrahend
 *
 * Overflow low when b > 0 and a < INT64_MIN + b.
 * Overflow high when b < 0 and a > INT64_MAX + b.
 */
int64_t
gj_i64_sat_sub_u(int64_t i64A, int64_t i64B)
{
	(void)NULL;
	return b2908_sub_sat(i64A, i64B);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_sat_sub_u(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_sat_sub_u")));
