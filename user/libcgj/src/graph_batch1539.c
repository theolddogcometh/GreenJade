/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1539: min-width padded int64_t decimal format.
 *
 * Surface (unique symbols):
 *   int gj_format_i64_dec_w(int64_t n, char *out, size_t cap, size_t width);
 *     — Write the base-10 representation of n into out as a
 *       NUL-terminated string (optional leading '-'), left-padded with
 *       spaces to at least width characters (spaces before the sign).
 *       Returns characters written excluding NUL on success, or -1.
 *   int __gj_format_i64_dec_w  (alias)
 *   __libcgj_batch1539_marker = "libcgj-batch1539"
 *
 * Distinct from gj_format_i64_dec (batch447) — no min-width padding.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1539_marker[] = "libcgj-batch1539";

/* INT64_MIN needs 19 digits; reverse buffer holds digit chars. */
#define B1539_MAX_DIGITS 19

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_i64_dec_w — signed decimal with min space-padded field width.
 *
 * INT64_MIN is handled via two's-complement magnitude (no signed
 * negation of INT64_MIN). Padding spaces precede the optional sign.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   width 0 → natural form only
 *   natural length longer than width → no pad
 *   result does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   -42, width=6 → "   -42"
 *    42, width=4 → "  42"
 *     0, width=3 → "  0"
 */
int
gj_format_i64_dec_w(int64_t i64N, char *pOut, size_t cbCap, size_t cWidth)
{
	char aDig[B1539_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uMag;
	int fNeg;
	size_t cbBody;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	fNeg = 0;
	if (i64N < 0) {
		fNeg = 1;
		/* Two's-complement magnitude; safe for INT64_MIN. */
		uMag = (uint64_t)0 - (uint64_t)i64N;
	} else {
		uMag = (uint64_t)i64N;
	}

	nDig = 0;
	if (uMag == 0ull) {
		aDig[0] = '0';
		nDig = 1;
		fNeg = 0;
	} else {
		while (uMag > 0ull && nDig < B1539_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uMag % 10ull));
			uMag /= 10ull;
		}
	}

	/* sign (optional) + digits */
	cbBody = (size_t)nDig + (fNeg ? 1u : 0u);
	cbField = cbBody;
	if (cWidth > cbField) {
		cbField = cWidth;
	}
	cbNeed = cbField + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (cWidth > cbBody) {
		iPad = cWidth - cbBody;
		while (iPad > 0u) {
			pOut[iOut++] = ' ';
			iPad--;
		}
	}
	if (fNeg) {
		pOut[iOut++] = '-';
	}
	for (iDig = nDig; iDig > 0; iDig--) {
		pOut[iOut++] = aDig[iDig - 1];
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_i64_dec_w(int64_t i64N, char *pOut, size_t cbCap, size_t cWidth)
    __attribute__((alias("gj_format_i64_dec_w")));
