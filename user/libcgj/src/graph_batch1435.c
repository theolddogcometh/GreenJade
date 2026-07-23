/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1435: freestanding month/day → day-of-year.
 *
 * Surface (unique symbols):
 *   int gj_yday_from_md(unsigned uY, unsigned uM, unsigned uD);
 *     — Convert Gregorian (uY, uM, uD) to day-of-year (1..365/366).
 *       Returns -1 if the date is invalid (uM outside 1..12, or uD
 *       outside 1..days_in_month). Distinct from gj_ymd_to_yday
 *       (batch483) — unique symbol, same calendar role.
 *   int __gj_yday_from_md  (alias)
 *   __libcgj_batch1435_marker = "libcgj-batch1435"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Local month/leap tables; does not call other batches.
 */

#include <stdint.h>

const char __libcgj_batch1435_marker[] = "libcgj-batch1435";

/* Days in each non-leap month, index 1..12 (index 0 unused). */
static const unsigned char b1435_mdays[13] = {
	0u,
	31u, 28u, 31u, 30u, 31u, 30u,
	31u, 31u, 30u, 31u, 30u, 31u
};

/* Cumulative days before month m in a non-leap year (index 1..12). */
static const unsigned short b1435_cum[13] = {
	0u,
	0u, 31u, 59u, 90u, 120u, 151u,
	181u, 212u, 243u, 273u, 304u, 334u
};

static int
b1435_is_leap(unsigned uY)
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

static unsigned
b1435_days_in_month(unsigned uY, unsigned uM)
{
	unsigned uDays;

	if (uM < 1u || uM > 12u) {
		return 0u;
	}
	uDays = (unsigned)b1435_mdays[uM];
	if (uM == 2u && b1435_is_leap(uY)) {
		uDays = 29u;
	}
	return uDays;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_yday_from_md — day-of-year for Gregorian (y, m, d).
 *
 * uY: year (leap affects February and post-February yday)
 * uM: month 1..12
 * uD: day of month 1..days_in_month(y, m)
 *
 * Returns 1..366 on success, -1 if uM or uD is out of range.
 * Example: 2000-03-01 → 61 (leap); 1900-03-01 → 60 (not leap).
 */
int
gj_yday_from_md(unsigned uY, unsigned uM, unsigned uD)
{
	unsigned uMax;
	unsigned uYday;

	uMax = b1435_days_in_month(uY, uM);
	if (uMax == 0u || uD < 1u || uD > uMax) {
		return -1;
	}

	uYday = (unsigned)b1435_cum[uM] + uD;
	if (uM > 2u && b1435_is_leap(uY)) {
		uYday += 1u;
	}
	return (int)uYday;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_yday_from_md(unsigned uY, unsigned uM, unsigned uD)
    __attribute__((alias("gj_yday_from_md")));
