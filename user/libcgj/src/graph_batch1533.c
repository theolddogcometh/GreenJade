/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1533: bounded hex parse → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_hex_n(const char *s, size_t n, uint64_t *out);
 *     — Parse an ASCII hexadecimal integer from at most n characters of
 *       s into *out. Optional "0x" / "0X" prefix is accepted when it
 *       fits in the window. Stops at the first non-hex digit or after n
 *       chars. Does not require a trailing NUL (unlike gj_parse_u64_hex).
 *       On success write value and return the number of hex digits
 *       consumed (prefix not counted). On failure return -1 (no write).
 *   int __gj_parse_u64_hex_n  (alias)
 *   __libcgj_batch1533_marker = "libcgj-batch1533"
 *
 * Distinct from gj_parse_u64_hex (batch392) — full-string match only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1533_marker[] = "libcgj-batch1533";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1533_hex_digit(char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a') + 10;
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64_hex_n — bounded hexadecimal ASCII → uint64_t.
 *
 * Grammar (prefix of at most n chars):
 *   ( "0x" | "0X" )? [0-9a-fA-F]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   n == 0 / no digits / "0x" alone / overflow → -1
 *   trailing non-hex within the window stops the parse (OK if ≥1 digit)
 */
int
gj_parse_u64_hex_n(const char *szIn, size_t cMax, uint64_t *pOut)
{
	const char *pCur;
	size_t iPos;
	uint64_t uVal;
	int nDig;
	int nDigitVal;

	if (szIn == NULL || pOut == NULL || cMax == 0u) {
		return -1;
	}

	pCur = szIn;
	iPos = 0u;

	/* Optional 0x / 0X prefix (must leave room for at least one digit). */
	if (iPos + 1u < cMax && pCur[0] == '0' &&
	    (pCur[1] == 'x' || pCur[1] == 'X')) {
		pCur += 2;
		iPos += 2u;
	}

	if (iPos >= cMax) {
		return -1;
	}
	nDigitVal = b1533_hex_digit(*pCur);
	if (nDigitVal < 0) {
		return -1;
	}

	uVal = 0ull;
	nDig = 0;
	while (iPos < cMax && (nDigitVal = b1533_hex_digit(*pCur)) >= 0) {
		if (uVal > (UINT64_MAX >> 4)) {
			return -1;
		}
		uVal = (uVal << 4) | (uint64_t)nDigitVal;
		pCur++;
		iPos++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}

	*pOut = uVal;
	return nDig;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_u64_hex_n(const char *szIn, size_t cMax, uint64_t *pOut)
    __attribute__((alias("gj_parse_u64_hex_n")));
