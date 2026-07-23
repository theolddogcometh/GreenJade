/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2046: three-way signum of int64_t.
 *
 * Surface (unique symbols):
 *   int gj_i64_signum(int64_t x);
 *     - Returns -1 if x < 0, 0 if x == 0, +1 if x > 0.
 *   int __gj_i64_signum  (alias)
 *   __libcgj_batch2046_marker = "libcgj-batch2046"
 *
 * Post-2000 i64 arith exclusive wave (2041-2050). Distinct from
 * gj_signum_i64 (batch379) and gj_i64_sign (batch614) - unique
 * gj_i64_signum surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2046_marker[] = "libcgj-batch2046";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2046_signum - sign of x as -1, 0, or +1.
 */
static int
b2046_signum(int64_t i64X)
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
 * gj_i64_signum - sign of x as -1, 0, or +1.
 *
 * Branch form avoids arithmetic right-shift assumptions and INT64_MIN
 * negation overflow; comparisons are exact for the full int64_t range.
 */
int
gj_i64_signum(int64_t i64X)
{
	(void)NULL;
	return b2046_signum(i64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i64_signum(int64_t i64X)
    __attribute__((alias("gj_i64_signum")));
