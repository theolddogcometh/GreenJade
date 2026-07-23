/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2487: min-width zero-padded lowercase hex.
 *
 * Surface (unique symbols):
 *   int gj_format_u64_hex_min(uint64_t n, char *out, size_t cap,
 *                             size_t min_width);
 *     — Write n as lowercase hex without a "0x" prefix, left-padded
 *       with '0' to at least min_width characters. Minimal significant
 *       digits (n == 0 → "0" when min_width <= 1). Returns characters
 *       written excluding NUL, or -1.
 *   int __gj_format_u64_hex_min  (alias)
 *   __libcgj_batch2487_marker = "libcgj-batch2487"
 *
 * Distinct from gj_format_u64_hex (batch393) and gj_format_u64_hex_w
 * (batch1537, space pad). Post-2480 parse exclusive wave (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2487_marker[] = "libcgj-batch2487";

#define B2487_MAX_DIGITS 16

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2487_hex_digits(uint64_t uVal, char *pTmp, int nTmpCap)
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
 * gj_format_u64_hex_min — lowercase hex with min zero-padded width.
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   min_width 0 → natural digits only
 *   natural digits longer than min_width → no pad
 *   result does not fit → -1
 * Success: length excluding NUL.
 *
 * Examples:
 *   255, min_width=4 → "00ff"
 *   0,   min_width=2 → "00"
 */
int
gj_format_u64_hex_min(uint64_t u64N, char *pOut, size_t cbCap, size_t cMin)
{
	char aDig[B2487_MAX_DIGITS];
	int nDig;
	int iDig;
	size_t cbField;
	size_t cbNeed;
	size_t iOut;
	size_t iPad;

	(void)NULL;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	nDig = b2487_hex_digits(u64N, aDig, B2487_MAX_DIGITS);
	if (nDig <= 0) {
		return -1;
	}

	cbField = (size_t)nDig;
	if (cMin > cbField) {
		cbField = cMin;
	}
	cbNeed = cbField + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (cMin > (size_t)nDig) {
		iPad = cMin - (size_t)nDig;
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

int __gj_format_u64_hex_min(uint64_t u64N, char *pOut, size_t cbCap,
    size_t cMin)
    __attribute__((alias("gj_format_u64_hex_min")));
