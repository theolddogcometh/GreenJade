/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1534: bounded octal parse → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_oct_n(const char *s, size_t n, uint64_t *out);
 *     — Parse an ASCII octal integer from at most n characters of s
 *       into *out. Digits are [0-7] only (no "0o" prefix). Stops at the
 *       first non-octal digit or after n chars. Does not require a
 *       trailing NUL. On success write value and return the number of
 *       octal digits consumed. On failure return -1 (no write).
 *   int __gj_parse_u64_oct_n  (alias)
 *   __libcgj_batch1534_marker = "libcgj-batch1534"
 *
 * Unique surface; no prior gj_parse_u64_oct full-string twin in the tree.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1534_marker[] = "libcgj-batch1534";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64_oct_n — bounded octal ASCII → uint64_t.
 *
 * Grammar (prefix of at most n chars):
 *   [0-7]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   n == 0 / no digits / overflow past UINT64_MAX → -1
 *   trailing non-octal within the window stops the parse (OK if ≥1 digit)
 *   leading '+' / '-' / whitespace / "0o" prefix not accepted
 *
 * Examples (n large enough):
 *   "755x" → 493, return 3
 *   "0"    → 0, return 1
 *   "17" with n=1 → 1, return 1
 */
int
gj_parse_u64_oct_n(const char *szIn, size_t cMax, uint64_t *pOut)
{
	const char *pCur;
	size_t iPos;
	uint64_t uVal;
	int nDig;

	if (szIn == NULL || pOut == NULL || cMax == 0u) {
		return -1;
	}

	pCur = szIn;
	iPos = 0u;
	if (iPos >= cMax || *pCur < '0' || *pCur > '7') {
		return -1;
	}

	uVal = 0ull;
	nDig = 0;
	while (iPos < cMax && *pCur >= '0' && *pCur <= '7') {
		uint64_t uDig = (uint64_t)(*pCur - '0');

		/* overflow if u would exceed UINT64_MAX after <<3 | d */
		if (uVal > (UINT64_MAX >> 3)) {
			return -1;
		}
		uVal = (uVal << 3) | uDig;
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

int __gj_parse_u64_oct_n(const char *szIn, size_t cMax, uint64_t *pOut)
    __attribute__((alias("gj_parse_u64_oct_n")));
