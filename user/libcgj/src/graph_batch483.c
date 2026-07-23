/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch483: freestanding Y-M-D → day-of-year.
 *
 * Surface (unique symbols):
 *   int gj_ymd_to_yday(unsigned y, unsigned m, unsigned d);
 *     — Convert Gregorian calendar date (y, m, d) to day-of-year.
 *       Returns 1..365 (common) or 1..366 (leap), or -1 if the date
 *       is invalid (m outside 1..12, or d outside 1..days_in_month).
 *   int __gj_ymd_to_yday  (alias)
 *   __libcgj_batch483_marker = "libcgj-batch483"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Month/leap tables are local so
 * this TU stays self-contained (does not call batch481/482).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch483_marker[] = "libcgj-batch483";

/* Days in each non-leap month, index 1..12 (index 0 unused). */
static const unsigned char b483_mdays[13] = {
	0u,
	31u, 28u, 31u, 30u, 31u, 30u,
	31u, 31u, 30u, 31u, 30u, 31u
};

/* Proleptic Gregorian leap: same rule as gj_is_leap_year (batch481). */
static int
b483_is_leap(unsigned y)
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

static unsigned
b483_days_in_month(unsigned y, unsigned m)
{
	unsigned nDays;

	if (m < 1u || m > 12u) {
		return 0u;
	}
	nDays = (unsigned)b483_mdays[m];
	if (m == 2u && b483_is_leap(y)) {
		nDays = 29u;
	}
	return nDays;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ymd_to_yday — day-of-year for Gregorian (y, m, d).
 *
 * y: year (leap affects February and cumulative day count)
 * m: month 1..12
 * d: day of month 1..days_in_month(y, m)
 *
 * Returns 1..366 on success, -1 if m or d is out of range.
 * Example: 2000-03-01 → 61 (leap); 1900-03-01 → 60 (not leap).
 */
int
gj_ymd_to_yday(unsigned y, unsigned m, unsigned d)
{
	unsigned nMax;
	unsigned nYday;
	unsigned i;

	nMax = b483_days_in_month(y, m);
	if (nMax == 0u || d < 1u || d > nMax) {
		return -1;
	}

	nYday = 0u;
	for (i = 1u; i < m; i++) {
		nYday += b483_days_in_month(y, i);
	}
	nYday += d;

	return (int)nYday;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ymd_to_yday(unsigned y, unsigned m, unsigned d)
    __attribute__((alias("gj_ymd_to_yday")));
