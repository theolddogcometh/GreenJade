/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1734: decimal digit count for int64_t magnitude.
 *
 * Surface (unique symbols):
 *   unsigned gj_decimal_digit_count_i64(int64_t n);
 *     — Number of base-10 digits in |n| (sign is not counted). Zero
 *       has 1 digit. Range is [1, 19]; INT64_MIN magnitude has 19 digits
 *       (9223372036854775808).
 *   unsigned __gj_decimal_digit_count_i64  (alias)
 *   __libcgj_batch1734_marker = "libcgj-batch1734"
 *
 * Decimal/fixed-print exclusive. Distinct from gj_decimal_digit_count_u64
 * (batch1733). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1734_marker[] = "libcgj-batch1734";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1734_count_u64 — digit ladder for non-negative magnitude in u64.
 * Max magnitude for int64 is 19 digits, so 20-digit branch is unused
 * for this surface but kept for complete thresholds.
 */
static unsigned
b1734_count_u64(uint64_t u64N)
{
	if (u64N < 10ULL) {
		return 1u;
	}
	if (u64N < 100ULL) {
		return 2u;
	}
	if (u64N < 1000ULL) {
		return 3u;
	}
	if (u64N < 10000ULL) {
		return 4u;
	}
	if (u64N < 100000ULL) {
		return 5u;
	}
	if (u64N < 1000000ULL) {
		return 6u;
	}
	if (u64N < 10000000ULL) {
		return 7u;
	}
	if (u64N < 100000000ULL) {
		return 8u;
	}
	if (u64N < 1000000000ULL) {
		return 9u;
	}
	if (u64N < 10000000000ULL) {
		return 10u;
	}
	if (u64N < 100000000000ULL) {
		return 11u;
	}
	if (u64N < 1000000000000ULL) {
		return 12u;
	}
	if (u64N < 10000000000000ULL) {
		return 13u;
	}
	if (u64N < 100000000000000ULL) {
		return 14u;
	}
	if (u64N < 1000000000000000ULL) {
		return 15u;
	}
	if (u64N < 10000000000000000ULL) {
		return 16u;
	}
	if (u64N < 100000000000000000ULL) {
		return 17u;
	}
	if (u64N < 1000000000000000000ULL) {
		return 18u;
	}
	return 19u;
}

/*
 * b1734_abs_u64 — |n| as uint64_t without UB on INT64_MIN.
 */
static uint64_t
b1734_abs_u64(int64_t i64N)
{
	uint64_t u64Mag;

	if (i64N >= 0) {
		return (uint64_t)i64N;
	}
	/* Two's-complement: -(INT64_MIN) is not representable as int64_t. */
	u64Mag = (uint64_t)(-(i64N + 1)) + 1ull;
	return u64Mag;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_decimal_digit_count_i64 — base-10 digits of |n|; 1 when n == 0.
 *
 * Sign is not included in the count (print helpers add '-' separately).
 */
unsigned
gj_decimal_digit_count_i64(int64_t i64N)
{
	return b1734_count_u64(b1734_abs_u64(i64N));
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_decimal_digit_count_i64(int64_t i64N)
    __attribute__((alias("gj_decimal_digit_count_i64")));
