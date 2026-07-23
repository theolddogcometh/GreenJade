/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch389: decimal digit count for uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_digit_count_u64(uint64_t n);
 *     — Number of base-10 digits in n. gj_digit_count_u64(0) == 1
 *       (the single digit '0'). Range is [1, 20]; UINT64_MAX has 20
 *       digits (18446744073709551615).
 *   unsigned __gj_digit_count_u64  (alias)
 *   __libcgj_batch389_marker = "libcgj-batch389"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch389_marker[] = "libcgj-batch389";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digit_count_u64 — base-10 digit length of n; 1 when n == 0.
 *
 * Threshold ladder avoids division and is O(1) with a fixed compare
 * depth. Boundaries are exact powers of ten through 10^19.
 */
unsigned
gj_digit_count_u64(uint64_t u64N)
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

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_digit_count_u64(uint64_t u64N)
    __attribute__((alias("gj_digit_count_u64")));
