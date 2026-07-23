/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4189: Luhn check digit for a uint32_t value.
 *
 * Surface (unique symbols):
 *   uint32_t gj_luhn_u32_u(uint32_t x);
 *     - Luhn check digit (0..9) for the decimal digits of x.
 *       Rightmost payload digit is doubled (check digit is appended).
 *       x == 0 is treated as the single digit '0'.
 *   uint32_t __gj_luhn_u32_u  (alias)
 *   __libcgj_batch4189_marker = "libcgj-batch4189"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190). Unique gj_luhn_u32_u surface only; no multi-def.
 * Distinct from gj_luhn_check_digit / gj_luhn_check_u (ASCII strings).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4189_marker[] = "libcgj-batch4189";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Luhn check digit for decimal digits of x.
 * Walk from the least-significant digit (rightmost of payload = doubled).
 */
static uint32_t
b4189_luhn(uint32_t u32X)
{
	uint32_t u32Sum;
	uint32_t u32D;
	int fDouble;

	u32Sum = 0u;
	fDouble = 1;
	do {
		u32D = u32X % 10u;
		u32X /= 10u;
		if (fDouble != 0) {
			u32D *= 2u;
			if (u32D > 9u) {
				u32D -= 9u;
			}
		}
		u32Sum += u32D;
		fDouble = (fDouble != 0) ? 0 : 1;
	} while (u32X != 0u);

	return (10u - (u32Sum % 10u)) % 10u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_luhn_u32_u - Luhn check digit (0..9) for decimal digits of x.
 *
 * x: unsigned 32-bit integer whose decimal form is the Luhn payload
 *
 * Returns the single check digit in 0..9. Always succeeds (x has a
 * well-defined decimal representation including 0). No parent wires.
 */
uint32_t
gj_luhn_u32_u(uint32_t u32X)
{
	(void)NULL;
	return b4189_luhn(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_luhn_u32_u(uint32_t u32X)
    __attribute__((alias("gj_luhn_u32_u")));
