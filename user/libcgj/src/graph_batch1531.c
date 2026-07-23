/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1531: bounded decimal parse → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_dec_n(const char *s, size_t n, uint64_t *out);
 *     — Parse an ASCII decimal integer from at most n characters of s
 *       into *out. Stops at the first non-digit or after n chars.
 *       Does not require a trailing NUL within the window (unlike
 *       gj_parse_u64_dec). On success write value and return the number
 *       of digits consumed. On failure return -1 (no write).
 *   int __gj_parse_u64_dec_n  (alias)
 *   __libcgj_batch1531_marker = "libcgj-batch1531"
 *
 * Distinct from gj_parse_u64_dec (batch390) — full-string match only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1531_marker[] = "libcgj-batch1531";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64_dec_n — bounded decimal ASCII → uint64_t.
 *
 * Grammar (prefix of at most n chars):
 *   [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   n == 0 / no digits / overflow past UINT64_MAX → -1
 *   trailing non-digits within the window stop the parse (OK if ≥1 digit)
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples (n large enough):
 *   "42x"  → 42, return 2
 *   "0"    → 0, return 1
 *   "12" with n=1 → 1, return 1
 */
int
gj_parse_u64_dec_n(const char *szIn, size_t cMax, uint64_t *pOut)
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
	if (iPos >= cMax || *pCur < '0' || *pCur > '9') {
		return -1;
	}

	uVal = 0ull;
	nDig = 0;
	while (iPos < cMax && *pCur >= '0' && *pCur <= '9') {
		uint64_t uDig = (uint64_t)(*pCur - '0');

		if (uVal > (UINT64_MAX - uDig) / 10ull) {
			return -1;
		}
		uVal = uVal * 10ull + uDig;
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

int __gj_parse_u64_dec_n(const char *szIn, size_t cMax, uint64_t *pOut)
    __attribute__((alias("gj_parse_u64_dec_n")));
