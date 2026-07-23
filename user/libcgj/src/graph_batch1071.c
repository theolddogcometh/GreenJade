/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1071: zero-padded fixed-width u64 decimal.
 *
 * Surface (unique symbols):
 *   int gj_str_from_u64_fixed(uint64_t v, char *out, size_t cap, size_t width);
 *     — Write v as a base-10 ASCII string of exactly width digits,
 *       zero-padded on the left, then NUL. If the natural digit count
 *       exceeds width, write the full unpadded digits (no truncation)
 *       when they fit. Returns written length excluding NUL, or -1.
 *   int __gj_str_from_u64_fixed  (alias)
 *   __libcgj_batch1071_marker = "libcgj-batch1071"
 *
 * Distinct from gj_format_u64_dec (batch391) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1071_marker[] = "libcgj-batch1071";

#define B1071_MAX_DIGITS 20

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_from_u64_fixed — zero-padded fixed-width decimal of u64V.
 *
 * pOut:  destination buffer
 * cbCap: capacity including room for NUL
 * cbWidth: minimum field width (0 allowed; then natural digits only)
 *
 * Edge cases:
 *   pOut == NULL or cbCap == 0 → -1
 *   result (max(digits, width) + NUL) does not fit → -1
 * Success: length excluding NUL.
 */
int
gj_str_from_u64_fixed(uint64_t u64V, char *pOut, size_t cbCap, size_t cbWidth)
{
	char aDig[B1071_MAX_DIGITS];
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

	uVal = u64V;
	nDig = 0;
	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B1071_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uVal % 10ull));
			uVal /= 10ull;
		}
	}

	cbField = (size_t)nDig;
	if (cbWidth > cbField) {
		cbField = cbWidth;
	}
	cbNeed = cbField + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	if (cbWidth > (size_t)nDig) {
		iPad = cbWidth - (size_t)nDig;
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

int __gj_str_from_u64_fixed(uint64_t u64V, char *pOut, size_t cbCap, size_t cbWidth)
    __attribute__((alias("gj_str_from_u64_fixed")));
