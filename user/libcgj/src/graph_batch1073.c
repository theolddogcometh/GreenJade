/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1073: uint32_t → lowercase hex ASCII.
 *
 * Surface (unique symbols):
 *   int gj_str_from_hex_u32(uint32_t v, char *out, size_t cap);
 *     — Write v as lowercase hex without "0x" and without leading zeros
 *       (except v == 0 → "0"). Returns length excluding NUL, or -1.
 *   int __gj_str_from_hex_u32  (alias)
 *   __libcgj_batch1073_marker = "libcgj-batch1073"
 *
 * Distinct from gj_format_u64_hex / gj_format_hex_u64 — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1073_marker[] = "libcgj-batch1073";

#define B1073_MAX_DIGITS 8

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_from_hex_u32 — lowercase hex digits of u32V into pOut.
 */
int
gj_str_from_hex_u32(uint32_t u32V, char *pOut, size_t cbCap)
{
	char aDig[B1073_MAX_DIGITS];
	int nDig;
	int iDig;
	uint32_t uVal;
	size_t iOut;
	size_t cbNeed;
	unsigned uNibble;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	uVal = u32V;
	nDig = 0;
	if (uVal == 0u) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0u && nDig < B1073_MAX_DIGITS) {
			uNibble = (unsigned)(uVal & 0xfu);
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

int __gj_str_from_hex_u32(uint32_t u32V, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_from_hex_u32")));
