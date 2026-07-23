/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1433: freestanding Gregorian leap-year test.
 *
 * Surface (unique symbols):
 *   int gj_is_leap_year_gregorian(unsigned uY);
 *     — Return 1 if year uY is a leap year under the proleptic
 *       Gregorian rule, else 0.
 *       Leap iff (uY % 4 == 0 && uY % 100 != 0) || (uY % 400 == 0).
 *       Year 0 is divisible by 400 → leap (proleptic).
 *   int __gj_is_leap_year_gregorian  (alias)
 *   __libcgj_batch1433_marker = "libcgj-batch1433"
 *
 * Distinct from gj_is_leap_year (batch481) — avoid multi-def. Same
 * mathematical rule; exclusive calendar-helper surface for this band.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1433_marker[] = "libcgj-batch1433";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_leap_year_gregorian — proleptic Gregorian leap-year predicate.
 *
 * uY: year (unsigned; 0 is leap under the 400-year rule)
 *
 * Returns 1 if leap, 0 otherwise.
 * Examples: 2000 → 1, 1900 → 0, 2024 → 1, 2023 → 0.
 */
int
gj_is_leap_year_gregorian(unsigned uY)
{
	if ((uY % 400u) == 0u) {
		return 1;
	}
	if ((uY % 100u) == 0u) {
		return 0;
	}
	if ((uY % 4u) == 0u) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_leap_year_gregorian(unsigned uY)
    __attribute__((alias("gj_is_leap_year_gregorian")));
