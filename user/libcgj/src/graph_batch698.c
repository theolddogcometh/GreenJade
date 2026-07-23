/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch698: freestanding (sec, nsec) → milliseconds.
 *
 * Surface (unique symbols):
 *   int64_t gj_duration_ms(int64_t sec, int32_t nsec);
 *     — Convert a timespec-shaped duration (seconds + nanoseconds) to
 *       whole milliseconds using mathematical floor (toward −∞):
 *         floor(sec * 1000 + nsec / 1_000_000)
 *       Equivalent to floor((sec * 1e9 + nsec) / 1e6) when the product
 *       is representable. Saturates to INT64_MIN / INT64_MAX if the
 *       scaled sum would overflow int64_t. Does not normalize nsec
 *       range; any int32_t nsec is accepted.
 *   int64_t __gj_duration_ms  (alias)
 *   __libcgj_batch698_marker = "libcgj-batch698"
 *
 * Distinct from gj_secs_to_ms / gj_ms_to_secs (batch484/485, unsigned
 * whole-second only) and from gj_duration_parse* (string parse). Unique
 * gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timespec. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch698_marker[] = "libcgj-batch698";

/* Nanoseconds per millisecond. */
#define B698_NSEC_PER_MS  1000000ll

/* Largest |sec| with |sec| * 1000 still in int64 range (trunc toward 0). */
#define B698_I64_MAX      0x7fffffffffffffffLL
#define B698_I64_MIN      (-B698_I64_MAX - 1)
#define B698_MAX_SECS_POS (B698_I64_MAX / 1000ll)
#define B698_MAX_SECS_NEG (B698_I64_MIN / 1000ll)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b698_floor_div — floor(a / b) for b > 0 (mathematical floor, not trunc).
 *
 * C99 signed division truncates toward zero. When a < 0 and a is not
 * divisible by b, subtract one from the quotient to move toward −∞.
 */
static int64_t
b698_floor_div(int64_t a, int64_t b)
{
	int64_t q;
	int64_t r;

	/* b > 0 is guaranteed by callers. */
	q = a / b;
	r = a % b;
	if (r != (int64_t)0 && a < (int64_t)0) {
		q -= (int64_t)1;
	}
	return q;
}

/*
 * b698_add_sat — saturating addition of two int64_t values.
 */
static int64_t
b698_add_sat(int64_t a, int64_t b)
{
	if (b > (int64_t)0) {
		if (a > B698_I64_MAX - b) {
			return B698_I64_MAX;
		}
	} else if (b < (int64_t)0) {
		if (a < B698_I64_MIN - b) {
			return B698_I64_MIN;
		}
	}
	return a + b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_duration_ms — (sec, nsec) → whole milliseconds (floor).
 *
 * sec:  whole seconds (signed)
 * nsec: nanosecond remainder / adjustment (any int32_t; not range-checked)
 *
 * Computes floor(sec * 1000 + nsec / 1e6). The nsec contribution uses
 * mathematical floor division by 1_000_000. The sec contribution is
 * exact when |sec| <= B698_MAX_SECS_*; otherwise the result saturates
 * (INT64_MAX for large positive, INT64_MIN for large negative). Adding
 * the nsec floor term is also saturating.
 *
 * Examples (normalized nsec in 0..999999999):
 *   (0, 0)           → 0
 *   (1, 0)           → 1000
 *   (1, 500000)      → 1000
 *   (1, 1500000)     → 1001
 *   (0, 999999)      → 0
 *   (0, 1000000)     → 1
 *   (-1, 0)          → -1000
 *   (0, -1)          → -1   (floor of −1e-6 ms)
 *   (0, -1000000)    → -1
 *   (0, -1000001)    → -2
 */
int64_t
gj_duration_ms(int64_t sec, int32_t nsec)
{
	int64_t ms_sec;
	int64_t ms_nsec;

	/* floor(nsec / 1_000_000) — nsec is int32 so this never overflows. */
	ms_nsec = b698_floor_div((int64_t)nsec, B698_NSEC_PER_MS);

	/* sec * 1000 with saturation. */
	if (sec > B698_MAX_SECS_POS) {
		return B698_I64_MAX;
	}
	if (sec < B698_MAX_SECS_NEG) {
		return B698_I64_MIN;
	}
	ms_sec = sec * 1000ll;

	return b698_add_sat(ms_sec, ms_nsec);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_duration_ms(int64_t sec, int32_t nsec)
    __attribute__((alias("gj_duration_ms")));
