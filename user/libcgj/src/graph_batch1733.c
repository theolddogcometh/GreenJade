/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1733: decimal digit count for uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_decimal_digit_count_u64(uint64_t n);
 *     — Number of base-10 digits in n. Zero has 1 digit ('0').
 *       Range is [1, 20]; UINT64_MAX has 20 digits.
 *   unsigned __gj_decimal_digit_count_u64  (alias)
 *   __libcgj_batch1733_marker = "libcgj-batch1733"
 *
 * Distinct from gj_digit_count_u64 (batch389) — decimal/fixed-print
 * exclusive surface name for this wave. Unique; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1733_marker[] = "libcgj-batch1733";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1733_pow10_thresholds — O(1) ladder of exact powers of ten.
 * Returns digit count in [1, 20].
 */
static unsigned
b1733_count(uint64_t u64N)
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
	if (u64N < 10000000000000000000ULL) {
		return 19u;
	}
	return 20u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_decimal_digit_count_u64 — base-10 digit length of n; 1 when n == 0.
 */
unsigned
gj_decimal_digit_count_u64(uint64_t u64N)
{
	return b1733_count(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_decimal_digit_count_u64(uint64_t u64N)
    __attribute__((alias("gj_decimal_digit_count_u64")));
