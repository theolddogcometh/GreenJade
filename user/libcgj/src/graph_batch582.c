/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch582 (renamed surface to avoid multi-def with earlier batch): decimal digit palindrome test for uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_palindrome_p_u64(uint64_t x);
 *     — 1 if the base-10 digit sequence of x is a palindrome, else 0.
 *       Leading zeros are not part of the decimal representation (except
 *       that x == 0 is the single digit '0', which is a palindrome).
 *   __gj_palindrome_p_u64  (alias)
 *   __libcgj_batch582_marker = "libcgj-batch582"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch582_marker[] = "libcgj-batch582";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_palindrome_p_u64 — true if decimal digits of x read the same forwards
 * and backwards.
 *
 * x: value under test
 *
 * Extract least-significant digit first into a fixed buffer (at most 20
 * digits for any uint64_t), then compare ends toward the center. Returns 1
 * on match, 0 otherwise. Single-digit values (including 0) are palindromes.
 */
int
gj_palindrome_p_u64(uint64_t x)
{
	uint8_t aDigits[20];
	unsigned cDigits;
	unsigned iLo;
	unsigned iHi;

	cDigits = 0u;
	do {
		aDigits[cDigits] = (uint8_t)(x % 10u);
		cDigits++;
		x /= 10u;
	} while (x != 0u);

	iLo = 0u;
	iHi = cDigits - 1u;
	while (iLo < iHi) {
		if (aDigits[iLo] != aDigits[iHi]) {
			return 0;
		}
		iLo++;
		iHi--;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_palindrome_p_u64(uint64_t x)
    __attribute__((alias("gj_palindrome_p_u64")));
