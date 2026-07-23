/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch926: saturating int64_t subtract.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_sat_sub(int64_t a, int64_t b);
 *     — a - b with saturation at INT64_MAX / INT64_MIN (no wrap).
 *   int64_t __gj_i64_sat_sub  (alias)
 *   __libcgj_batch926_marker = "libcgj-batch926"

 *
 * Does NOT define gj_sat_sub_u64 / gj_u64_sat_sub — signed path only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch926_marker[] = "libcgj-batch926";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_sat_sub — signed subtract; clamp to INT64_MAX/MIN on overflow.
 *
 * a - b overflows high when b is negative and a > INT64_MAX + b.
 * a - b overflows low when b is positive and a < INT64_MIN + b.
 */
int64_t
gj_i64_sat_sub(int64_t i64A, int64_t i64B)
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

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_sat_sub(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_sat_sub")));
