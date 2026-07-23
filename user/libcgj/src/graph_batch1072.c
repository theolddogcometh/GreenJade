/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1072: int64_t → decimal ASCII string.
 *
 * Surface (unique symbols):
 *   int gj_str_from_i64(int64_t v, char *out, size_t cap);
 *     — Write signed base-10 representation of v into out as a
 *       NUL-terminated string (optional leading '-', no leading zeros
 *       except for v == 0). Returns length excluding NUL, or -1.
 *   int __gj_str_from_i64  (alias)
 *   __libcgj_batch1072_marker = "libcgj-batch1072"
 *
 * Distinct from gj_format_i64_dec (batch447) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1072_marker[] = "libcgj-batch1072";

#define B1072_MAX_DIGITS 19

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_from_i64 — write signed decimal digits of i64V into pOut.
 *
 * INT64_MIN handled via two's-complement magnitude (no signed negation).
 *
 * Edge cases:
 *   pOut == NULL or cbCap == 0 → -1
 *   cap too small for sign + digits + NUL → -1
 * Success: length excluding NUL.
 */
int
gj_str_from_i64(int64_t i64V, char *pOut, size_t cbCap)
{
	char aDig[B1072_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uMag;
	int fNeg;
	size_t iOut;
	size_t cbNeed;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	fNeg = 0;
	if (i64V < 0) {
		fNeg = 1;
		uMag = (uint64_t)0 - (uint64_t)i64V;
	} else {
		uMag = (uint64_t)i64V;
	}

	nDig = 0;
	if (uMag == 0ull) {
		aDig[0] = '0';
		nDig = 1;
		fNeg = 0;
	} else {
		while (uMag > 0ull && nDig < B1072_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uMag % 10ull));
			uMag /= 10ull;
		}
	}

	cbNeed = (size_t)nDig + 1u + (fNeg ? 1u : 0u);
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
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

int __gj_str_from_i64(int64_t i64V, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_from_i64")));
