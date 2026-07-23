/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch393: format uint64_t as hexadecimal text
 * (no "0x" prefix).
 *
 * Surface (unique symbols):
 *   int gj_format_u64_hex(uint64_t n, char *out, size_t cap, int upper);
 *     — Write a NUL-terminated hex rendering of n into out. Digits are
 *       the minimal significant hex length (no leading zeros except for
 *       n == 0, which is "0"). upper != 0 selects A-F; otherwise a-f.
 *       No "0x" / "0X" prefix is emitted.
 *         0            → "0"
 *         255 upper=0  → "ff"
 *         255 upper=1  → "FF"
 *         UINT64_MAX   → "ffffffffffffffff" / "FFFFFFFFFFFFFFFF"
 *       Returns the number of characters written excluding NUL on
 *       success, or -1 on error (out == NULL, cap == 0, or result
 *       including NUL does not fit in cap). On error out is undefined.
 *   __gj_format_u64_hex  (alias)
 *   __libcgj_batch393_marker = "libcgj-batch393"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch393_marker[] = "libcgj-batch393";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b393_u64_hex_digits — write hex digits of uVal into aTmp reversed.
 * Returns digit count (at least 1 for zero). aTmp must hold >= 16 bytes.
 * fUpper selects A-F vs a-f.
 */
static int
b393_u64_hex_digits(uint64_t uVal, char *aTmp, int nTmpCap, int fUpper)
{
	int nDig = 0;
	char chA;

	chA = fUpper ? 'A' : 'a';

	if (uVal == 0ull) {
		if (nTmpCap < 1) {
			return 0;
		}
		aTmp[0] = '0';
		return 1;
	}

	while (uVal > 0ull && nDig < nTmpCap) {
		unsigned uNibble;

		uNibble = (unsigned)(uVal & 0xfull);
		if (uNibble < 10u) {
			aTmp[nDig] = (char)('0' + (int)uNibble);
		} else {
			aTmp[nDig] = (char)(chA + (int)(uNibble - 10u));
		}
		nDig++;
		uVal >>= 4;
	}
	return nDig;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_format_u64_hex — format n as hex without a 0x prefix.
 *
 * Minimal significant digits; zero is a single "0". upper selects case
 * of A-F / a-f only (does not affect the '0'..'9' digits).
 *
 * Edge cases:
 *   out == NULL or cap == 0 → -1
 *   cap too small for digits + NUL → -1
 * Success: length excluding NUL.
 */
int
gj_format_u64_hex(uint64_t n, char *out, size_t cap, int upper)
{
	char aDig[16];
	int nDig;
	int iDig;
	size_t iOut;
	size_t cbNeed;
	int fUpper;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	fUpper = (upper != 0) ? 1 : 0;
	nDig = b393_u64_hex_digits(n, aDig, (int)sizeof(aDig), fUpper);
	if (nDig <= 0) {
		return -1;
	}

	/* digits + NUL */
	cbNeed = (size_t)nDig + 1u;
	if (cap < cbNeed) {
		return -1;
	}

	iOut = 0u;
	for (iDig = nDig; iDig > 0; iDig--) {
		out[iOut++] = aDig[iDig - 1];
	}
	out[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_format_u64_hex(uint64_t n, char *out, size_t cap, int upper)
    __attribute__((alias("gj_format_u64_hex")));
