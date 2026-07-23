/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1432: freestanding serial day → civil y/m/d.
 *
 * Surface (unique symbols):
 *   int gj_civil_from_days(int64_t i64Z, int *pY, unsigned *pM, unsigned *pD);
 *     — Inverse of gj_days_from_civil: convert days since 1970-01-01
 *       into proleptic Gregorian year/month/day. On success writes
 *       *pY, *pM (1..12), *pD (1..31) and returns 0. NULL out-params
 *       → -1. Clean-room reimplementation of the Howard Hinnant
 *       civil-from-days algorithm (no third-party source copied).
 *   int __gj_civil_from_days  (alias)
 *   __libcgj_batch1432_marker = "libcgj-batch1432"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>
#include <stddef.h>

const char __libcgj_batch1432_marker[] = "libcgj-batch1432";

/*
 * b1432_civil_from_days — Hinnant-style civil y/m/d from serial day.
 *
 * i64Z is days since 1970-01-01. Uses C99 toward-zero division with
 * the 146096 bias so negative eras match the published algorithm.
 */
static void
b1432_civil_from_days(int64_t i64Z, int *pY, unsigned *pM, unsigned *pD)
{
	int64_t i64ZAdj;
	int64_t i64Era;
	unsigned uDoe;
	unsigned uYoe;
	int64_t i64Y;
	unsigned uDoy;
	unsigned uMp;
	unsigned uD;
	unsigned uM;

	i64ZAdj = i64Z + 719468;
	if (i64ZAdj >= 0) {
		i64Era = i64ZAdj / 146097;
	} else {
		i64Era = (i64ZAdj - 146096) / 146097;
	}

	uDoe = (unsigned)(i64ZAdj - i64Era * 146097); /* [0, 146096] */
	uYoe = (uDoe - uDoe / 1460u + uDoe / 36524u - uDoe / 146096u) / 365u;
	i64Y = (int64_t)uYoe + i64Era * 400;
	uDoy = uDoe - (365u * uYoe + uYoe / 4u - uYoe / 100u); /* [0, 365] */
	uMp = (5u * uDoy + 2u) / 153u; /* [0, 11] */
	uD = uDoy - (153u * uMp + 2u) / 5u + 1u; /* [1, 31] */
	uM = (uMp < 10u) ? (uMp + 3u) : (uMp - 9u); /* [1, 12] */
	if (uM <= 2u) {
		i64Y += 1;
	}

	*pY = (int)i64Y;
	*pM = uM;
	*pD = uD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_civil_from_days — civil Gregorian date from days since 1970-01-01.
 *
 * i64Z: serial day (0 = 1970-01-01)
 * pY/pM/pD: out year, month 1..12, day 1..31 (must be non-NULL)
 *
 * Returns 0 on success, -1 if any out pointer is NULL.
 * Example: 0 → 1970-01-01; 1 → 1970-01-02; -1 → 1969-12-31.
 */
int
gj_civil_from_days(int64_t i64Z, int *pY, unsigned *pM, unsigned *pD)
{
	if (pY == NULL || pM == NULL || pD == NULL) {
		return -1;
	}
	b1432_civil_from_days(i64Z, pY, pM, pD);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_civil_from_days(int64_t i64Z, int *pY, unsigned *pM, unsigned *pD)
    __attribute__((alias("gj_civil_from_days")));
