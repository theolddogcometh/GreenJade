/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2143: bounded signed decimal parse -> int32_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_i32_dec_n(const char *s, size_t n, int32_t *out);
 *     - Parse an optional-signed ASCII decimal integer from at most n
 *       characters of s into *out. Stops at the first non-digit after the
 *       optional sign, or after n chars. Does not require a trailing NUL
 *       within the window (unlike gj_parse_i32). On success write value
 *       and return the number of digits consumed (sign not counted).
 *       On failure return -1 (no write).
 *   int __gj_parse_i32_dec_n  (alias)
 *   __libcgj_batch2143_marker = "libcgj-batch2143"
 *
 * Distinct from gj_parse_i32 (batch1076) and gj_parse_i64_dec_n
 * (batch1532). Post-2140 parse exclusive wave (2141-2150).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2143_marker[] = "libcgj-batch2143";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i32_dec_n - bounded optional-signed decimal ASCII -> int32_t.
 *
 * Grammar (prefix of at most n chars):
 *   [+|-]? [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL -> -1
 *   n == 0 / no digits / overflow past int32 range -> -1
 *   sign alone / trailing junk after digits stops parse (OK if >=1 digit)
 *   whitespace not accepted
 *   INT32_MIN / INT32_MAX accepted; one past either -> -1
 *
 * Examples (n large enough):
 *   "-42x" -> -42, return 2
 *   "+0"   -> 0, return 1
 *   "12" with n=1 -> 1, return 1
 */
int
gj_parse_i32_dec_n(const char *szIn, size_t cMax, int32_t *pOut)
{
	const char *pCur;
	size_t iPos;
	uint32_t uMag;
	uint32_t uLimit;
	int nDig;
	int fNeg;

	(void)NULL;

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

	/* Magnitude limit: INT32_MAX, or INT32_MIN magnitude when negative. */
	uLimit = (uint32_t)INT32_MAX;
	if (fNeg) {
		uLimit = (uint32_t)INT32_MAX + 1u;
	}

	uMag = 0u;
	nDig = 0;
	while (iPos < cMax && *pCur >= '0' && *pCur <= '9') {
		uint32_t uDig = (uint32_t)(*pCur - '0');

		if (uMag > (uLimit - uDig) / 10u) {
			return -1;
		}
		uMag = uMag * 10u + uDig;
		pCur++;
		iPos++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}

	if (fNeg) {
		if (uMag == (uint32_t)INT32_MAX + 1u) {
			*pOut = INT32_MIN;
		} else {
			*pOut = -(int32_t)uMag;
		}
	} else {
		*pOut = (int32_t)uMag;
	}
	return nDig;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_i32_dec_n(const char *szIn, size_t cMax, int32_t *pOut)
    __attribute__((alias("gj_parse_i32_dec_n")));
