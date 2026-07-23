/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2489: min-width zero-padded int64_t decimal.
 *
 * Surface (unique symbols):
 *   int gj_format_i64_dec_min(int64_t n, char *out, size_t cap,
 *                             size_t min_width);
 *     — Write the base-10 representation of n into out as a
 *       NUL-terminated string (optional leading '-'), left-padded with
 *       '0' after the sign to at least min_width total characters
 *       (printf-style %0*d field width). Returns characters written
 *       excluding NUL on success, or -1.
 *   int __gj_format_i64_dec_min  (alias)
 *   __libcgj_batch2489_marker = "libcgj-batch2489"
 *
 * Distinct from gj_format_i64_dec (batch447) and gj_format_i64_dec_w
 * (batch1539, space pad). Post-2480 parse exclusive wave (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2489_marker[] = "libcgj-batch2489";

/* INT64_MIN needs 19 digits; reverse buffer holds digit chars. */
#define B2489_MAX_DIGITS 19

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_i64_dec_min — signed decimal with min zero-padded field width.
 *
 * INT64_MIN is handled via two's-complement magnitude (no signed
 * negation of INT64_MIN). Zero pad sits between the optional '-' and
 * the significant digits (printf %0*d style). Field width includes sign.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   min_width 0 → natural form only
 *   natural length longer than min_width → no pad
 *   result does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   -42, min_width=6 → "-00042"
 *    42, min_width=4 → "0042"
 *     0, min_width=3 → "000"
 */
int
gj_format_i64_dec_min(int64_t i64N, char *pOut, size_t cbCap, size_t cMin)
{
	char aDig[B2489_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uMag;
	int fNeg;
	size_t cbBody;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;
	size_t cPad;

	(void)NULL;

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
		while (uMag > 0ull && nDig < B2489_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uMag % 10ull));
			uMag /= 10ull;
		}
	}

	/* sign (optional) + digits */
	cbBody = (size_t)nDig + (fNeg ? 1u : 0u);
	cbField = cbBody;
	if (cMin > cbField) {
		cbField = cMin;
	}
	cbNeed = cbField + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (fNeg) {
		pOut[iOut++] = '-';
	}
	/* Zero-pad between sign and digits to fill min_width. */
	if (cMin > cbBody) {
		cPad = cMin - cbBody;
		iPad = cPad;
		while (iPad > 0u) {
			pOut[iOut++] = '0';
			iPad--;
		}
	}
	for (iDig = nDig; iDig > 0; iDig--) {
		pOut[iOut++] = aDig[iDig - 1];
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_i64_dec_min(int64_t i64N, char *pOut, size_t cbCap,
    size_t cMin)
    __attribute__((alias("gj_format_i64_dec_min")));
