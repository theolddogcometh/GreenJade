/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch699: freestanding (sec, nsec) → microseconds.
 *
 * Surface (unique symbols):
 *   int64_t gj_duration_us(int64_t sec, int32_t nsec);
 *     — Convert a broken-down duration (seconds + nanoseconds) to a
 *       single int64_t microsecond count:
 *         us = sec * 1_000_000 + nsec / 1000
 *       Division of nsec truncates toward zero (C99). Product and sum
 *       saturate at INT64_MAX / INT64_MIN on overflow. nsec is accepted
 *       as a raw int32_t (not required to be in [0, 999999999]).
 *   int64_t __gj_duration_us  (alias)
 *   __libcgj_batch699_marker = "libcgj-batch699"
 *
 * Distinct from gj_duration_parse / gj_duration_format (batch148 string
 * surface), gj_duration_parse_sec (batch278), gj_secs_to_ms (batch484),
 * and gj_ms_to_secs (batch485) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch699_marker[] = "libcgj-batch699";

/* Microseconds in one whole second. */
#define B699_US_PER_SEC  1000000LL

/* Largest |sec| whose sec * B699_US_PER_SEC still fits in int64_t. */
#define B699_MAX_SECS  (INT64_MAX / B699_US_PER_SEC)
#define B699_MIN_SECS  (INT64_MIN / B699_US_PER_SEC)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_duration_us — (sec, nsec) → total microseconds, saturating.
 *
 * sec:  whole seconds (signed)
 * nsec: additional nanoseconds (signed int32; truncated to us via /1000)
 *
 * Returns sec * 1000000 + nsec / 1000 when the result fits in int64_t.
 * On multiply or add overflow, saturates to INT64_MAX (positive side) or
 * INT64_MIN (negative side). Does not set errno.
 */
int64_t
gj_duration_us(int64_t sec, int32_t nsec)
{
	int64_t i64FromSec;
	int64_t i64FromNsec;

	i64FromNsec = (int64_t)nsec / 1000LL;

	/* sec * 1e6 with saturation */
	if (sec > B699_MAX_SECS) {
		return INT64_MAX;
	}
	if (sec < B699_MIN_SECS) {
		return INT64_MIN;
	}
	i64FromSec = sec * B699_US_PER_SEC;

	/* add nsec contribution with saturation */
	if (i64FromNsec > 0) {
		if (i64FromSec > INT64_MAX - i64FromNsec) {
			return INT64_MAX;
		}
	} else if (i64FromNsec < 0) {
		if (i64FromSec < INT64_MIN - i64FromNsec) {
			return INT64_MIN;
		}
	}

	return i64FromSec + i64FromNsec;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_duration_us(int64_t sec, int32_t nsec)
    __attribute__((alias("gj_duration_us")));
