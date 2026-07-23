/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2264: freestanding timeval (sec, usec) to
 * whole milliseconds (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   int64_t gj_timeval_to_ms(int64_t sec, int32_t usec);
 *     - Convert a timeval-shaped duration (seconds + microseconds) to
 *       whole milliseconds using mathematical floor (toward -inf):
 *         floor(sec * 1000 + usec / 1000)
 *       Saturates to INT64_MIN / INT64_MAX if the scaled sum would
 *       overflow int64_t. Does not normalize usec range; any int32_t
 *       usec is accepted.
 *   int64_t __gj_timeval_to_ms  (alias)
 *   __libcgj_batch2264_marker = "libcgj-batch2264"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_duration_ms (batch698, nsec) and gj_time_us_to_ms (batch1034,
 * flat us). Unique gj_timeval_to_ms surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timeval. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2264_marker[] = "libcgj-batch2264";

/* Microseconds per millisecond. */
#define B2264_USEC_PER_MS  1000ll

/* Largest |sec| with |sec| * 1000 still in int64 range. */
#define B2264_I64_MAX      0x7fffffffffffffffLL
#define B2264_I64_MIN      (-B2264_I64_MAX - 1)
#define B2264_MAX_SECS_POS (B2264_I64_MAX / 1000ll)
#define B2264_MAX_SECS_NEG (B2264_I64_MIN / 1000ll)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2264_floor_div - floor(a / b) for b > 0 (mathematical floor, not trunc).
 */
static int64_t
b2264_floor_div(int64_t a, int64_t b)
{
	int64_t q;
	int64_t r;

	q = a / b;
	r = a % b;
	if (r != (int64_t)0 && a < (int64_t)0) {
		q -= (int64_t)1;
	}
	return q;
}

/*
 * b2264_add_sat - saturating addition of two int64_t values.
 */
static int64_t
b2264_add_sat(int64_t a, int64_t b)
{
	if (b > (int64_t)0) {
		if (a > B2264_I64_MAX - b) {
			return B2264_I64_MAX;
		}
	} else if (b < (int64_t)0) {
		if (a < B2264_I64_MIN - b) {
			return B2264_I64_MIN;
		}
	}
	return a + b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeval_to_ms - (sec, usec) to whole milliseconds (floor).
 *
 * sec:  whole seconds (signed)
 * usec: microsecond remainder / adjustment (any int32_t; not range-checked)
 *
 * Computes floor(sec * 1000 + usec / 1000). The usec contribution uses
 * mathematical floor division by 1000. The sec contribution is exact
 * when |sec| <= B2264_MAX_SECS_*; otherwise the result saturates.
 *
 * Examples (normalized usec in 0..999999):
 *   (0, 0)        -> 0
 *   (1, 0)        -> 1000
 *   (1, 500)      -> 1000
 *   (1, 1500)     -> 1001
 *   (0, 999)      -> 0
 *   (0, 1000)     -> 1
 *   (-1, 0)       -> -1000
 *   (0, -1)       -> -1
 *   (0, -1000)    -> -1
 *   (0, -1001)    -> -2
 */
int64_t
gj_timeval_to_ms(int64_t sec, int32_t usec)
{
	int64_t ms_sec;
	int64_t ms_usec;

	(void)NULL;

	/* floor(usec / 1000) - usec is int32 so this never overflows. */
	ms_usec = b2264_floor_div((int64_t)usec, B2264_USEC_PER_MS);

	/* sec * 1000 with saturation. */
	if (sec > B2264_MAX_SECS_POS) {
		return B2264_I64_MAX;
	}
	if (sec < B2264_MAX_SECS_NEG) {
		return B2264_I64_MIN;
	}
	ms_sec = sec * 1000ll;

	return b2264_add_sat(ms_sec, ms_usec);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_timeval_to_ms(int64_t sec, int32_t usec)
    __attribute__((alias("gj_timeval_to_ms")));
