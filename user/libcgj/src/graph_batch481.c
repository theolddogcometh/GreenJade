/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch481: freestanding Gregorian leap-year test.
 *
 * Surface (unique symbols):
 *   int gj_is_leap_year(unsigned y);
 *     — Return 1 if year y is a leap year under the proleptic Gregorian
 *       rule, else 0.
 *       Leap iff (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0).
 *       Year 0 is divisible by 400 → leap (proleptic).
 *   int __gj_is_leap_year  (alias)
 *   __libcgj_batch481_marker = "libcgj-batch481"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch481_marker[] = "libcgj-batch481";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_leap_year — proleptic Gregorian leap-year predicate.
 *
 * y: year (unsigned; 0 is leap under the 400-year rule)
 *
 * Returns 1 if leap, 0 otherwise.
 */
int
gj_is_leap_year(unsigned y)
{
	if ((y % 400u) == 0u) {
		return 1;
	}
	if ((y % 100u) == 0u) {
		return 0;
	}
	if ((y % 4u) == 0u) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_leap_year(unsigned y)
    __attribute__((alias("gj_is_leap_year")));
