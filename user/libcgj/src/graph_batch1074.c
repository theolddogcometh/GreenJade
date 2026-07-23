/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1074: uint64_t → lowercase hex ASCII.
 *
 * Surface (unique symbols):
 *   int gj_str_from_hex_u64(uint64_t v, char *out, size_t cap);
 *     — Write v as lowercase hex without "0x" and without leading zeros
 *       (except v == 0 → "0"). Returns length excluding NUL, or -1.
 *   int __gj_str_from_hex_u64  (alias)
 *   __libcgj_batch1074_marker = "libcgj-batch1074"
 *
 * Distinct from gj_format_u64_hex / gj_format_hex_u64 — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1074_marker[] = "libcgj-batch1074";

#define B1074_MAX_DIGITS 16

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_from_hex_u64 — lowercase hex digits of u64V into pOut.
 */
int
gj_str_from_hex_u64(uint64_t u64V, char *pOut, size_t cbCap)
{
	char aDig[B1074_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t iOut;
	size_t cbNeed;
	unsigned uNibble;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	uVal = u64V;
	nDig = 0;
	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B1074_MAX_DIGITS) {
			uNibble = (unsigned)(uVal & 0xfull);
			if (uNibble < 10u) {
				aDig[nDig] = (char)('0' + (int)uNibble);
			} else {
				aDig[nDig] = (char)('a' + (int)(uNibble - 10u));
			}
			nDig++;
			uVal >>= 4;
		}
	}

	cbNeed = (size_t)nDig + 1u;
	if (cbCap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	for (iDig = nDig; iDig > 0; iDig--) {
		pOut[iOut++] = aDig[iDig - 1];
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_from_hex_u64(uint64_t u64V, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_from_hex_u64")));
