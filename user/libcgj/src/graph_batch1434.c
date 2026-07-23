/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1434: freestanding days-in-month (Gregorian).
 *
 * Surface (unique symbols):
 *   unsigned gj_days_in_month_gregorian(unsigned uY, unsigned uM);
 *     — Return the number of days in month uM (1..12) of year uY under
 *       the proleptic Gregorian calendar. February is 29 in leap years
 *       and 28 otherwise. Return 0 if uM is outside 1..12.
 *   unsigned __gj_days_in_month_gregorian  (alias)
 *   __libcgj_batch1434_marker = "libcgj-batch1434"
 *
 * Note: batch482 already owns gj_days_in_month / __gj_days_in_month.
 * This TU uses the _gregorian suffix (parallel to batch1433
 * gj_is_leap_year_gregorian) so the symbol stays unique.
 * Local leap rule — does not call batch481/1433.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1434_marker[] = "libcgj-batch1434";

/* Days in each non-leap month, index 1..12 (index 0 unused). */
static const unsigned char b1434_mdays[13] = {
	0u,
	31u, 28u, 31u, 30u, 31u, 30u,
	31u, 31u, 30u, 31u, 30u, 31u
};

/* Proleptic Gregorian leap (local; self-contained TU). */
static int
b1434_is_leap(unsigned uY)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_days_in_month_gregorian — days in month uM of year uY (Gregorian).
 *
 * uY: year (used only for February leap adjustment)
 * uM: month 1..12; any other value → 0
 *
 * Returns 28..31 on success, 0 if uM is invalid.
 */
unsigned
gj_days_in_month_gregorian(unsigned uY, unsigned uM)
{
	unsigned uDays;

	if (uM < 1u || uM > 12u) {
		return 0u;
	}

	uDays = (unsigned)b1434_mdays[uM];
	if (uM == 2u && b1434_is_leap(uY)) {
		uDays = 29u;
	}
	return uDays;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_days_in_month_gregorian(unsigned uY, unsigned uM)
    __attribute__((alias("gj_days_in_month_gregorian")));
