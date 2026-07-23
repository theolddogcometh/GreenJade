/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2483: full-string hex string → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_hex_full(const char *s, uint64_t *out);
 *     — Parse a full-string ASCII hexadecimal integer into *out.
 *       Optional "0x" / "0X" prefix is accepted.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_u64_hex_full  (alias)
 *   __libcgj_batch2483_marker = "libcgj-batch2483"
 *
 * Distinct from gj_parse_u64_hex (batch392). Post-2480 parse exclusive
 * wave (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2483_marker[] = "libcgj-batch2483";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2483_hex_digit — value 0..15 for hex char, or -1 if not hex.
 */
static int
b2483_hex_digit(char ch)
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
 * gj_parse_u64_hex_full — full-string hexadecimal ASCII → uint64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   ( "0x" | "0X" )? [0-9a-fA-F]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-hex junk / overflow past UINT64_MAX → -1
 *   "0x" alone (prefix with no digits) → -1
 *   "0", "00", "0x0", "0X00" → 0
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"                    → 0
 *   "ff" / "FF" / "0xff"   → 255
 *   "0xFFFFFFFFFFFFFFFF"   → UINT64_MAX
 *   "0x10000000000000000"  → -1 (overflow)
 *   "" / "x" / "0x" / "1g" / " 1" → -1
 */
int
gj_parse_u64_hex_full(const char *sz, uint64_t *pOut)
{
	const char *pCur;
	uint64_t uVal;
	int nDig;
	int nDigit;

	(void)NULL;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}

	pCur = sz;

	/* Optional 0x / 0X prefix (must be followed by at least one digit). */
	if (pCur[0] == '0' && (pCur[1] == 'x' || pCur[1] == 'X')) {
		pCur += 2;
	}

	nDigit = b2483_hex_digit(*pCur);
	if (nDigit < 0) {
		return -1;
	}

	uVal = 0ull;
	nDig = 0;
	while ((nDigit = b2483_hex_digit(*pCur)) >= 0) {
		/* overflow if uVal would exceed UINT64_MAX after <<4 | digit */
		if (uVal > (UINT64_MAX >> 4)) {
			return -1;
		}
		uVal = (uVal << 4) | (uint64_t)nDigit;
		pCur++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}
	if (*pCur != '\0') {
		return -1;
	}

	*pOut = uVal;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_u64_hex_full(const char *sz, uint64_t *pOut)
    __attribute__((alias("gj_parse_u64_hex_full")));
