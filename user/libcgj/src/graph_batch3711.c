/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3711: uint64 decimal format buffer-need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_u64_to_dec_need_u(uint64_t v);
 *     - Bytes needed (including trailing NUL) for the minimal decimal
 *       ASCII rendering of v (no leading zeros except for v == 0 → "0").
 *       Range is [2, 21]; UINT64_MAX needs 21 (20 digits + NUL).
 *   size_t __gj_u64_to_dec_need_u  (alias)
 *   __libcgj_batch3711_marker = "libcgj-batch3711"
 *
 * Exclusive continuum CREATE-ONLY (3711-3720). Distinct from
 * gj_digit_count_u64 (batch389 digits only), gj_decimal_digit_count_u64
 * (batch1733), gj_format_u64_dec (batch391 write) — length-only need
 * surface including NUL; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3711_marker[] = "libcgj-batch3711";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3711_dec_digits — base-10 digit length of u; 1 when u == 0.
 * Threshold ladder; O(1) compare depth. Range [1, 20].
 */
static size_t
b3711_dec_digits(uint64_t u)
{
	if (u < 10ULL) {
		return 1u;
	}
	if (u < 100ULL) {
		return 2u;
	}
	if (u < 1000ULL) {
		return 3u;
	}
	if (u < 10000ULL) {
		return 4u;
	}
	if (u < 100000ULL) {
		return 5u;
	}
	if (u < 1000000ULL) {
		return 6u;
	}
	if (u < 10000000ULL) {
		return 7u;
	}
	if (u < 100000000ULL) {
		return 8u;
	}
	if (u < 1000000000ULL) {
		return 9u;
	}
	if (u < 10000000000ULL) {
		return 10u;
	}
	if (u < 100000000000ULL) {
		return 11u;
	}
	if (u < 1000000000000ULL) {
		return 12u;
	}
	if (u < 10000000000000ULL) {
		return 13u;
	}
	if (u < 100000000000000ULL) {
		return 14u;
	}
	if (u < 1000000000000000ULL) {
		return 15u;
	}
	if (u < 10000000000000000ULL) {
		return 16u;
	}
	if (u < 100000000000000000ULL) {
		return 17u;
	}
	if (u < 1000000000000000000ULL) {
		return 18u;
	}
	if (u < 10000000000000000000ULL) {
		return 19u;
	}
	return 20u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_to_dec_need_u - bytes needed (incl. NUL) for decimal of v.
 *
 * Minimal significant digits; zero is "0" (need 2). Does not write a
 * buffer. No parent wires.
 *
 * Examples (return includes trailing NUL):
 *   0                  → 2   ("0")
 *   42                 → 3   ("42")
 *   18446744073709551615 → 21
 */
size_t
gj_u64_to_dec_need_u(uint64_t v)
{
	(void)NULL;
	return b3711_dec_digits(v) + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u64_to_dec_need_u(uint64_t v)
    __attribute__((alias("gj_u64_to_dec_need_u")));
