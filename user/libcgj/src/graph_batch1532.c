/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1532: bounded signed decimal parse → int64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_i64_dec_n(const char *s, size_t n, int64_t *out);
 *     — Parse an optional-signed ASCII decimal integer from at most n
 *       characters of s into *out. Stops at the first non-digit after the
 *       optional sign, or after n chars. Does not require a trailing NUL
 *       within the window (unlike gj_parse_i64_dec). On success write
 *       value and return the number of digits consumed (sign not counted).
 *       On failure return -1 (no write).
 *   int __gj_parse_i64_dec_n  (alias)
 *   __libcgj_batch1532_marker = "libcgj-batch1532"
 *
 * Distinct from gj_parse_i64_dec (batch446) — full-string match only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1532_marker[] = "libcgj-batch1532";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i64_dec_n — bounded optional-signed decimal ASCII → int64_t.
 *
 * Grammar (prefix of at most n chars):
 *   [+|-]? [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   n == 0 / no digits / overflow past int64 range → -1
 *   sign alone / trailing junk after digits stops parse (OK if ≥1 digit)
 *   whitespace not accepted
 *   INT64_MIN / INT64_MAX accepted; one past either → -1
 *
 * Examples (n large enough):
 *   "-42x" → -42, return 2
 *   "+0"   → 0, return 1
 *   "12" with n=1 → 1, return 1
 */
int
gj_parse_i64_dec_n(const char *szIn, size_t cMax, int64_t *pOut)
{
	const char *pCur;
	size_t iPos;
	uint64_t uMag;
	uint64_t uLimit;
	int nDig;
	int fNeg;

	if (szIn == NULL || pOut == NULL || cMax == 0u) {
		return -1;
	}

	pCur = szIn;
	iPos = 0u;
	fNeg = 0;

	if (iPos < cMax && (*pCur == '+' || *pCur == '-')) {
		if (*pCur == '-') {
			fNeg = 1;
		}
		pCur++;
		iPos++;
	}

	if (iPos >= cMax || *pCur < '0' || *pCur > '9') {
		return -1;
	}

	/* Magnitude limit: INT64_MAX, or INT64_MIN magnitude when negative. */
	uLimit = (uint64_t)INT64_MAX;
	if (fNeg) {
		uLimit = (uint64_t)INT64_MAX + 1ull;
	}

	uMag = 0ull;
	nDig = 0;
	while (iPos < cMax && *pCur >= '0' && *pCur <= '9') {
		uint64_t uDig = (uint64_t)(*pCur - '0');

		if (uMag > (uLimit - uDig) / 10ull) {
			return -1;
		}
		uMag = uMag * 10ull + uDig;
		pCur++;
		iPos++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}

	if (fNeg) {
		if (uMag == (uint64_t)INT64_MAX + 1ull) {
			*pOut = INT64_MIN;
		} else {
			*pOut = -(int64_t)uMag;
		}
	} else {
		*pOut = (int64_t)uMag;
	}
	return nDig;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_i64_dec_n(const char *szIn, size_t cMax, int64_t *pOut)
    __attribute__((alias("gj_parse_i64_dec_n")));
