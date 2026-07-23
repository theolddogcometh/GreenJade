/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1376: saturating int64_t subtract (continuum name).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_saturating_sub(int64_t a, int64_t b);
 *     — a - b with saturation at INT64_MAX / INT64_MIN (no wrap).
 *   int64_t __gj_i64_saturating_sub  (alias)
 *   __libcgj_batch1376_marker = "libcgj-batch1376"
 *
 * Distinct from gj_i64_sat_sub (batch926). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1376_marker[] = "libcgj-batch1376";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * a - b overflows high when b is negative and a > INT64_MAX + b.
 * a - b overflows low when b is positive and a < INT64_MIN + b.
 * Returns: 0 ok, +1 high sat, -1 low sat.
 */
static int
b1376_sub_ovf_dir(int64_t i64A, int64_t i64B)
{
	if (i64B > 0) {
		if (i64A < (INT64_MIN + i64B)) {
			return -1;
		}
	} else if (i64B < 0) {
		if (i64A > (INT64_MAX + i64B)) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_saturating_sub — signed subtract; clamp to INT64_MAX/MIN on overflow.
 */
int64_t
gj_i64_saturating_sub(int64_t i64A, int64_t i64B)
{
	int iDir;

	iDir = b1376_sub_ovf_dir(i64A, i64B);
	if (iDir > 0) {
		return INT64_MAX;
	}
	if (iDir < 0) {
		return INT64_MIN;
	}
	return i64A - i64B;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_saturating_sub(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_i64_saturating_sub")));
