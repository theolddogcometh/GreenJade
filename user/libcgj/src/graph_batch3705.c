/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3705: decimal digit count for uint32_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_u32_digit_count_u(uint32_t n);
 *     - Number of base-10 digits in n. gj_u32_digit_count_u(0) == 1
 *       (the single digit '0'). Range is [1, 10]; UINT32_MAX has 10
 *       digits (4294967295).
 *   unsigned __gj_u32_digit_count_u  (alias)
 *   __libcgj_batch3705_marker = "libcgj-batch3705"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_digit_count_u64 (batch389) and gj_decimal_digit_count_u64
 * (batch1733) — unique u32 digit_count_u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3705_marker[] = "libcgj-batch3705";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Base-10 digit length of n; 1 when n == 0. O(1) power-of-ten ladder.
 */
static unsigned
b3705_digit_count(uint32_t uN)
{
	if (uN < 10u) {
		return 1u;
	}
	if (uN < 100u) {
		return 2u;
	}
	if (uN < 1000u) {
		return 3u;
	}
	if (uN < 10000u) {
		return 4u;
	}
	if (uN < 100000u) {
		return 5u;
	}
	if (uN < 1000000u) {
		return 6u;
	}
	if (uN < 10000000u) {
		return 7u;
	}
	if (uN < 100000000u) {
		return 8u;
	}
	if (uN < 1000000000u) {
		return 9u;
	}
	return 10u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_digit_count_u - base-10 digit length of n; 1 when n == 0.
 *
 * n: unsigned 32-bit value
 *
 * Returns digit count in [1, 10]. No parent wires.
 */
unsigned
gj_u32_digit_count_u(uint32_t n)
{
	(void)NULL;
	return b3705_digit_count(n);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_u32_digit_count_u(uint32_t n)
    __attribute__((alias("gj_u32_digit_count_u")));
