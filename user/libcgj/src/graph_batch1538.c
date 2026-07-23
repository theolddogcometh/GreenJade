/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1538: min-width padded uppercase hex format.
 *
 * Surface (unique symbols):
 *   int gj_format_u64_hex_upper_w(uint64_t n, char *out, size_t cap,
 *                                 size_t width);
 *     — Write n as uppercase hex without a "0x" prefix, left-padded with
 *       spaces to at least width characters. Minimal significant digits
 *       (n == 0 → "0"). Returns characters written excluding NUL, or -1.
 *   int __gj_format_u64_hex_upper_w  (alias)
 *   __libcgj_batch1538_marker = "libcgj-batch1538"
 *
 * Distinct from gj_format_u64_hex (batch393) and gj_format_u64_hex_w
 * (batch1537, lowercase).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1538_marker[] = "libcgj-batch1538";

#define B1538_MAX_DIGITS 16

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1538_hex_digits_upper(uint64_t uVal, char *pTmp, int nTmpCap)
{
	int nDig = 0;
	unsigned uNibble;

	if (uVal == 0ull) {
		if (nTmpCap < 1) {
			return 0;
		}
		pTmp[0] = '0';
		return 1;
	}

	while (uVal > 0ull && nDig < nTmpCap) {
		uNibble = (unsigned)(uVal & 0xfull);
		if (uNibble < 10u) {
			pTmp[nDig] = (char)('0' + (int)uNibble);
		} else {
			pTmp[nDig] = (char)('A' + (int)(uNibble - 10u));
		}
		nDig++;
		uVal >>= 4;
	}
	return nDig;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_u64_hex_upper_w — uppercase hex with min space-padded width.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   width 0 → natural digits only
 *   natural digits longer than width → no pad
 *   result does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   255, width=4 → "  FF"
 *   0,   width=2 → " 0"
 */
int
gj_format_u64_hex_upper_w(uint64_t u64N, char *pOut, size_t cbCap,
    size_t cWidth)
{
	char aDig[B1538_MAX_DIGITS];
	int nDig;
	int iDig;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	nDig = b1538_hex_digits_upper(u64N, aDig, B1538_MAX_DIGITS);
	if (nDig <= 0) {
		return -1;
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

int __gj_format_u64_hex_upper_w(uint64_t u64N, char *pOut, size_t cbCap,
    size_t cWidth)
    __attribute__((alias("gj_format_u64_hex_upper_w")));
