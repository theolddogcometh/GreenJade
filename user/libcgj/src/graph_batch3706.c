/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3706: decimal digit count for uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u64_digit_count_u(uint64_t n);
 *     - Number of base-10 digits in n. gj_u64_digit_count_u(0) == 1
 *       (the single digit '0'). Range is [1, 20]; UINT64_MAX has 20
 *       digits (18446744073709551615).
 *   unsigned __gj_u64_digit_count_u  (alias)
 *   __libcgj_batch3706_marker = "libcgj-batch3706"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_digit_count_u64 (batch389), gj_decimal_digit_count_u64 (batch1733),
 * and gj_u32_digit_count_u (batch3705) — unique digit_count_u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3706_marker[] = "libcgj-batch3706";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Base-10 digit length of n; 1 when n == 0. O(1) power-of-ten ladder
 * through 10^19.
 */
static unsigned
b3706_digit_count(uint64_t u64N)
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
 * gj_u64_digit_count_u - base-10 digit length of n; 1 when n == 0.
 *
 * n: unsigned 64-bit value
 *
 * Returns digit count in [1, 20]. No parent wires.
 */
unsigned
gj_u64_digit_count_u(uint64_t n)
{
	(void)NULL;
	return b3706_digit_count(n);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u64_digit_count_u(uint64_t n)
    __attribute__((alias("gj_u64_digit_count_u")));
