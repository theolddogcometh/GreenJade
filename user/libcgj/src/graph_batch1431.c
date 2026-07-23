/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1431: freestanding civil y/m/d → serial day count.
 *
 * Surface (unique symbols):
 *   int64_t gj_days_from_civil(int nY, unsigned uM, unsigned uD);
 *     — Days since civil 1970-01-01 for Gregorian date (nY, uM, uD).
 *       Negative results are days before the Unix epoch. Month uM is
 *       1..12; day uD is 1..last_day_of_month. Invalid month → 0
 *       (same as the epoch date; callers must validate when needed).
 *       Clean-room reimplementation of the Howard Hinnant serial-day
 *       algorithm (proleptic Gregorian; no third-party source copied).
 *   int64_t __gj_days_from_civil  (alias)
 *   __libcgj_batch1431_marker = "libcgj-batch1431"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1431_marker[] = "libcgj-batch1431";

/*
 * b1431_days_from_civil — Hinnant-style serial day from civil y/m/d.
 *
 * Uses C99 toward-zero division with the standard era bias so the
 * result matches floor-division formulations on negative years.
 * Epoch offset 719468 maps the algorithm's internal day-0
 * (0000-03-01 civil, Mar-based year) onto 1970-01-01.
 */
static int64_t
b1431_days_from_civil(int nY, unsigned uM, unsigned uD)
{
	int64_t i64Y;
	int64_t i64Era;
	unsigned uYoe;
	unsigned uDoy;
	unsigned uDoe;

	/* Shift year so March is month 0 of a Mar–Feb year. */
	i64Y = (int64_t)nY;
	if (uM <= 2u) {
		i64Y -= 1;
	}

	if (i64Y >= 0) {
		i64Era = i64Y / 400;
	} else {
		i64Era = (i64Y - 399) / 400;
	}

	uYoe = (unsigned)(i64Y - i64Era * 400); /* [0, 399] */
	/* Day-of-year in Mar-based year: Mar=0 … Feb=11. */
	uDoy = (153u * (uM > 2u ? (uM - 3u) : (uM + 9u)) + 2u) / 5u + uD - 1u;
	uDoe = uYoe * 365u + uYoe / 4u - uYoe / 100u + uDoy; /* [0, 146096] */

	return i64Era * 146097 + (int64_t)uDoe - 719468;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_days_from_civil — days since 1970-01-01 for Gregorian (y, m, d).
 *
 * nY: year (proleptic Gregorian; may be negative)
 * uM: month 1..12
 * uD: day of month 1..28..31 (caller should keep in range)
 *
 * Returns serial day count relative to Unix civil epoch. uM outside
 * 1..12 → 0. Example: 1970-01-01 → 0; 1970-01-02 → 1; 1969-12-31 → -1.
 */
int64_t
gj_days_from_civil(int nY, unsigned uM, unsigned uD)
{
	if (uM < 1u || uM > 12u) {
		return 0;
	}
	if (uD < 1u || uD > 31u) {
		return 0;
	}
	return b1431_days_from_civil(nY, uM, uD);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_days_from_civil(int nY, unsigned uM, unsigned uD)
    __attribute__((alias("gj_days_from_civil")));
