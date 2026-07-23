/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2389: Luhn check-digit unique surface
 * (post-2380 checksum exclusive wave).
 *
 * Surface (unique symbols):
 *   int gj_luhn_check_digit(const char *digits);
 *     - Compute the Luhn check digit (0..9) for a payload of ASCII
 *       decimal digits at digits. Rightmost payload digit is treated
 *       as position that will be doubled once the check digit is
 *       appended (double from the right of the payload).
 *       NULL or empty or non-digit -> -1.
 *   int __gj_luhn_check_digit  (alias)
 *   __libcgj_batch2389_marker = "libcgj-batch2389"
 *
 * Post-2380 checksum exclusive wave (2381-2390). Unique Luhn surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2389_marker[] = "libcgj-batch2389";

/* ---- freestanding helpers ---------------------------------------------- */

/* Return 1 if ch is ASCII '0'..'9', else 0. */
static int
b2389_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/*
 * Compute Luhn check digit for a non-empty all-digit payload.
 * Walk from the right: first (rightmost) is doubled.
 */
static int
b2389_luhn_digit(const char *szDigits, size_t nLen)
{
	unsigned uSum;
	size_t i;
	int fDouble;

	uSum = 0u;
	fDouble = 1;
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
	return (int)((10u - (uSum % 10u)) % 10u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_luhn_check_digit - Luhn check digit for an ASCII digit payload.
 *
 * digits: NUL-terminated string of '0'..'9' (no check digit yet)
 *
 * Returns 0..9 on success, or -1 if digits is NULL, empty, or contains
 * a non-digit. Does not call libc.
 */
int
gj_luhn_check_digit(const char *szDigits)
{
	size_t nLen;

	(void)NULL;
	if (szDigits == NULL) {
		return -1;
	}
	nLen = 0u;
	while (szDigits[nLen] != '\0') {
		if (!b2389_is_digit(szDigits[nLen])) {
			return -1;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return -1;
	}
	return b2389_luhn_digit(szDigits, nLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_luhn_check_digit(const char *szDigits)
    __attribute__((alias("gj_luhn_check_digit")));
