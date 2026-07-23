/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1537: min-width padded lowercase hex format.
 *
 * Surface (unique symbols):
 *   int gj_format_u64_hex_w(uint64_t n, char *out, size_t cap, size_t width);
 *     — Write n as lowercase hex without a "0x" prefix, left-padded with
 *       spaces to at least width characters. Minimal significant digits
 *       (n == 0 → "0"). Returns characters written excluding NUL, or -1.
 *   int __gj_format_u64_hex_w  (alias)
 *   __libcgj_batch1537_marker = "libcgj-batch1537"
 *
 * Distinct from gj_format_u64_hex (batch393), gj_format_hex_u64
 * (batch586), and gj_format_u64_hex_upper_w (batch1538).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1537_marker[] = "libcgj-batch1537";

#define B1537_MAX_DIGITS 16

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1537_hex_digits(uint64_t uVal, char *pTmp, int nTmpCap)
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
			pTmp[nDig] = (char)('a' + (int)(uNibble - 10u));
		}
		nDig++;
		uVal >>= 4;
	}
	return nDig;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_u64_hex_w — lowercase hex with minimum space-padded width.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   width 0 → natural digits only
 *   natural digits longer than width → no pad
 *   result does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   255, width=4 → "  ff"
 *   0,   width=2 → " 0"
 */
int
gj_format_u64_hex_w(uint64_t u64N, char *pOut, size_t cbCap, size_t cWidth)
{
	char aDig[B1537_MAX_DIGITS];
	int nDig;
	int iDig;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	nDig = b1537_hex_digits(u64N, aDig, B1537_MAX_DIGITS);
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

int __gj_format_u64_hex_w(uint64_t u64N, char *pOut, size_t cbCap, size_t cWidth)
    __attribute__((alias("gj_format_u64_hex_w")));
