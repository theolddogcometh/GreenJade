/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch447: freestanding int64_t → decimal ASCII string.
 *
 * Surface (unique symbols):
 *   int gj_format_i64_dec(int64_t n, char *out, size_t cap);
 *     — Write the base-10 representation of n into out as a
 *       NUL-terminated string (optional leading '-', no leading zeros
 *       except for n == 0). Returns the number of characters written
 *       excluding NUL on success, or -1 on error (out == NULL, cap == 0,
 *       or result including NUL does not fit in cap). On error out is
 *       undefined.
 *   int __gj_format_i64_dec  (alias)
 *   __libcgj_batch447_marker = "libcgj-batch447"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch447_marker[] = "libcgj-batch447";

/* INT64_MIN needs '-' plus 19 digits; reverse buffer holds digit chars. */
#define B447_MAX_DIGITS 19

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_i64_dec — write signed decimal digits of n into out.
 *
 * Digits are emitted LSD-first into a stack temp, then reversed into out
 * so the result is MSD-first with no leading zeros (except zero itself).
 * Negative values get a leading '-'. INT64_MIN is handled via two's
 * complement magnitude (no signed negation of INT64_MIN).
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   cap too small for sign + digits + NUL → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   0          → "0"
 *   42         → "42"
 *   -1         → "-1"
 *   INT64_MAX  → "9223372036854775807"
 *   INT64_MIN  → "-9223372036854775808"
 */
int
gj_format_i64_dec(int64_t n, char *out, size_t cap)
{
	char aDig[B447_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uMag;
	int fNeg;
	size_t iOut;
	size_t cbNeed;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	fNeg = 0;
	if (n < 0) {
		fNeg = 1;
		/* Two's-complement magnitude; safe for INT64_MIN. */
		uMag = (uint64_t)0 - (uint64_t)n;
	} else {
		uMag = (uint64_t)n;
	}

	nDig = 0;
	if (uMag == 0ull) {
		aDig[0] = '0';
		nDig = 1;
		fNeg = 0;
	} else {
		while (uMag > 0ull && nDig < B447_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uMag % 10ull));
			uMag /= 10ull;
		}
	}

	/* optional sign + digits + NUL */
	cbNeed = (size_t)nDig + 1u + (fNeg ? 1u : 0u);
	if (cap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (fNeg) {
		out[iOut++] = '-';
	}
	for (iDig = nDig; iDig > 0; iDig--) {
		out[iOut++] = aDig[iDig - 1];
	}
	out[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_i64_dec(int64_t n, char *out, size_t cap)
    __attribute__((alias("gj_format_i64_dec")));
