/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2809: Luhn number validation (exclusive _u).
 *
 * Surface (unique symbols — do not export gj_luhn_check_digit):
 *   int gj_luhn_check_u(const char *digits);
 *     - Validate a full ASCII decimal digit string (including check
 *       digit) with the Luhn algorithm. Walk from the right: rightmost
 *       digit is NOT doubled (it is the check digit position).
 *       Returns 1 if valid, 0 if checksum fails, -1 if NULL/empty/
 *       non-digit.
 *   int __gj_luhn_check_u  (alias)
 *   __libcgj_batch2809_marker = "libcgj-batch2809"
 *
 * Distinct from batch2389 gj_luhn_check_digit (computes a new check
 * digit). This unit only defines the validation surface — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2809_marker[] = "libcgj-batch2809";

/* ---- freestanding helpers ---------------------------------------------- */

/* Return 1 if ch is ASCII '0'..'9', else 0. */
static int
b2809_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/*
 * Luhn validate a non-empty all-digit string (includes check digit).
 * Walk from the right: rightmost is position 1 (not doubled).
 * Returns 1 if sum % 10 == 0, else 0.
 */
static int
b2809_luhn_ok(const char *szDigits, size_t nLen)
{
	unsigned uSum;
	size_t i;
	int fDouble;

	uSum = 0u;
	fDouble = 0;
	i = nLen;
	while (i > 0u) {
		unsigned uD;

		i--;
		uD = (unsigned)(szDigits[i] - '0');
		if (fDouble) {
			uD *= 2u;
			if (uD > 9u) {
				uD -= 9u;
			}
		}
		uSum += uD;
		fDouble = fDouble ? 0 : 1;
	}
	return ((uSum % 10u) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_luhn_check_u - validate a full Luhn digit string.
 *
 * digits: NUL-terminated string of '0'..'9' including check digit
 *
 * Returns 1 if the Luhn checksum is valid, 0 if invalid, or -1 if
 * digits is NULL, empty, or contains a non-digit. Does not call libc.
 */
int
gj_luhn_check_u(const char *szDigits)
{
	size_t nLen;

	(void)NULL;
	if (szDigits == NULL) {
		return -1;
	}
	nLen = 0u;
	while (szDigits[nLen] != '\0') {
		if (!b2809_is_digit(szDigits[nLen])) {
			return -1;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return -1;
	}
	return b2809_luhn_ok(szDigits, nLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_luhn_check_u(const char *szDigits)
    __attribute__((alias("gj_luhn_check_u")));
