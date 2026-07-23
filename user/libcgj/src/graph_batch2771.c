/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2771: three-way sign of int64_t (exclusive
 * continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   int gj_i64_sign_u(int64_t x);
 *     - Returns -1 if x < 0, 0 if x == 0, +1 if x > 0.
 *   int __gj_i64_sign_u  (alias)
 *   __libcgj_batch2771_marker = "libcgj-batch2771"
 *
 * Distinct from gj_i64_sign (batch614) / gj_signum_i64 (batch379).
 * Unique gj_i64_sign_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2771_marker[] = "libcgj-batch2771";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Sign of x as -1, 0, or +1. Branch form avoids arithmetic right-shift
 * assumptions and INT64_MIN negation overflow.
 */
static int
b2771_sign(int64_t i64X)
{
	if (i64X < 0) {
		return -1;
	}
	if (i64X > 0) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_sign_u - sign of x as -1, 0, or +1.
 *
 * Comparisons are exact for the full int64_t range (including INT64_MIN).
 * Does not call libc.
 */
int
gj_i64_sign_u(int64_t i64X)
{
	(void)NULL;
	return b2771_sign(i64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i64_sign_u(int64_t i64X)
    __attribute__((alias("gj_i64_sign_u")));
