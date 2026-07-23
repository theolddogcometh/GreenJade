/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1439: freestanding Unix seconds → civil y/m/d.
 *
 * Surface (unique symbols):
 *   int gj_unix_to_civil_hint(int64_t i64Unix, int *pY, unsigned *pM,
 *                             unsigned *pD);
 *     — Break POSIX Unix seconds (UTC, leap seconds ignored) into a
 *       civil Gregorian year/month/day. Time-of-day is discarded via
 *       floor division by 86400. Writes *pY, *pM (1..12), *pD (1..31).
 *       Returns 0 on success, -1 if any out pointer is NULL.
 *       "hint" — calendar fields only; no tm, no timezone, no leap sec.
 *   int __gj_unix_to_civil_hint  (alias)
 *   __libcgj_batch1439_marker = "libcgj-batch1439"
 *
 * Local clean-room civil_from_days (Hinnant-style); does not call
 * batch1432. Distinct from private b94_unix_to_civil / b147 helpers.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>
#include <stddef.h>

const char __libcgj_batch1439_marker[] = "libcgj-batch1439";

/*
 * b1439_floor_div_86400 — floor(i64X / 86400) for any signed i64X.
 * C99 / truncates toward zero; adjust when x is negative and not
 * divisible so we get mathematical floor (needed for pre-epoch times).
 */
static int64_t
b1439_floor_div_86400(int64_t i64X)
{
	int64_t i64Q;

	i64Q = i64X / 86400;
	if (i64X < 0 && (i64X % 86400) != 0) {
		i64Q -= 1;
	}
	return i64Q;
}

/* Hinnant-style civil y/m/d from days since 1970-01-01 (local copy). */
static void
b1439_civil_from_days(int64_t i64Z, int *pY, unsigned *pM, unsigned *pD)
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

	uDoe = (unsigned)(i64ZAdj - i64Era * 146097);
	uYoe = (uDoe - uDoe / 1460u + uDoe / 36524u - uDoe / 146096u) / 365u;
	i64Y = (int64_t)uYoe + i64Era * 400;
	uDoy = uDoe - (365u * uYoe + uYoe / 4u - uYoe / 100u);
	uMp = (5u * uDoy + 2u) / 153u;
	uD = uDoy - (153u * uMp + 2u) / 5u + 1u;
	uM = (uMp < 10u) ? (uMp + 3u) : (uMp - 9u);
	if (uM <= 2u) {
		i64Y += 1;
	}

	*pY = (int)i64Y;
	*pM = uM;
	*pD = uD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unix_to_civil_hint — Unix seconds → civil Gregorian y/m/d (UTC).
 *
 * i64Unix: POSIX time_t-style seconds since 1970-01-01 00:00:00 UTC.
 *          Leap seconds are ignored (every day has 86400 SI seconds).
 * pY/pM/pD: out year, month 1..12, day 1..31
 *
 * Returns 0 on success, -1 if any out pointer is NULL.
 * Examples: 0 → 1970-01-01; 86400 → 1970-01-02; -1 → 1969-12-31.
 */
int
gj_unix_to_civil_hint(int64_t i64Unix, int *pY, unsigned *pM, unsigned *pD)
{
	int64_t i64Days;

	if (pY == NULL || pM == NULL || pD == NULL) {
		return -1;
	}

	i64Days = b1439_floor_div_86400(i64Unix);
	b1439_civil_from_days(i64Days, pY, pM, pD);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_unix_to_civil_hint(int64_t i64Unix, int *pY, unsigned *pM,
    unsigned *pD)
    __attribute__((alias("gj_unix_to_civil_hint")));
