/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch482: freestanding days-in-month (Gregorian).
 *
 * Surface (unique symbols):
 *   unsigned gj_days_in_month(unsigned y, unsigned m);
 *     — Return the number of days in month m (1..12) of year y under
 *       the proleptic Gregorian calendar. February is 29 in leap years
 *       and 28 otherwise. Return 0 if m is outside 1..12.
 *   unsigned __gj_days_in_month  (alias)
 *   __libcgj_batch482_marker = "libcgj-batch482"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Leap rule is local (no call into
 * batch481) so this TU stays self-contained.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch482_marker[] = "libcgj-batch482";

/* Days in each non-leap month, index 1..12 (index 0 unused). */
static const unsigned char b482_mdays[13] = {
	0u,
	31u, 28u, 31u, 30u, 31u, 30u,
	31u, 31u, 30u, 31u, 30u, 31u
};

/* Proleptic Gregorian leap: same rule as gj_is_leap_year (batch481). */
static int
b482_is_leap(unsigned y)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_days_in_month — days in month m of year y (Gregorian).
 *
 * y: year (used only for February leap adjustment)
 * m: month 1..12; any other value → 0
 *
 * Returns 28..31 on success, 0 if m is invalid.
 */
unsigned
gj_days_in_month(unsigned y, unsigned m)
{
	unsigned nDays;

	if (m < 1u || m > 12u) {
		return 0u;
	}

	nDays = (unsigned)b482_mdays[m];
	if (m == 2u && b482_is_leap(y)) {
		nDays = 29u;
	}
	return nDays;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_days_in_month(unsigned y, unsigned m)
    __attribute__((alias("gj_days_in_month")));
