/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1536: min-width padded uint64_t decimal format.
 *
 * Surface (unique symbols):
 *   int gj_format_u64_dec_w(uint64_t n, char *out, size_t cap, size_t width);
 *     — Write the base-10 representation of n into out as a
 *       NUL-terminated string, left-padded with spaces to at least
 *       width characters (printf-style min field width). No leading
 *       zeros except for n == 0. Returns characters written excluding
 *       NUL on success, or -1 on error.
 *   int __gj_format_u64_dec_w  (alias)
 *   __libcgj_batch1536_marker = "libcgj-batch1536"
 *
 * Distinct from gj_format_u64_dec (batch391) and gj_str_from_u64_fixed
 * (batch1071, zero-pad fixed width).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1536_marker[] = "libcgj-batch1536";

/* UINT64_MAX needs 20 decimal digits; reverse buffer holds that. */
#define B1536_MAX_DIGITS 20

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_u64_dec_w — decimal with minimum space-padded field width.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   width 0 → natural digits only (same as unpadded)
 *   natural digits longer than width → no pad, full digits
 *   result (max(digits, width) + NUL) does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   42, width=5 → "   42"
 *   42, width=1 → "42"
 *   0,  width=3 → "  0"
 */
int
gj_format_u64_dec_w(uint64_t u64N, char *pOut, size_t cbCap, size_t cWidth)
{
	char aDig[B1536_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	uVal = u64N;
	nDig = 0;
	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B1536_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uVal % 10ull));
			uVal /= 10ull;
		}
	}

	cbField = (size_t)nDig;
	if (cWidth > cbField) {
		cbField = cWidth;
	}
	cbNeed = cbField + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (cWidth > (size_t)nDig) {
		iPad = cWidth - (size_t)nDig;
		while (iPad > 0u) {
			pOut[iOut++] = ' ';
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

int __gj_format_u64_dec_w(uint64_t u64N, char *pOut, size_t cbCap, size_t cWidth)
    __attribute__((alias("gj_format_u64_dec_w")));
