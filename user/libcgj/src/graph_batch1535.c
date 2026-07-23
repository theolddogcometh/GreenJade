/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1535: bounded binary parse → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_bin_n(const char *s, size_t n, uint64_t *out);
 *     — Parse an ASCII binary integer from at most n characters of s
 *       into *out. Digits are [01] only (no "0b" prefix). Stops at the
 *       first non-binary digit or after n chars. Does not require a
 *       trailing NUL (unlike gj_parse_bin_u64). On success write value
 *       and return the number of bits/digits consumed. On failure
 *       return -1 (no write).
 *   int __gj_parse_u64_bin_n  (alias)
 *   __libcgj_batch1535_marker = "libcgj-batch1535"
 *
 * Distinct from gj_parse_bin_u64 (batch687) — full-string match only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1535_marker[] = "libcgj-batch1535";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64_bin_n — bounded binary ASCII → uint64_t.
 *
 * Grammar (prefix of at most n chars):
 *   [01]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   n == 0 / no bits / overflow past UINT64_MAX → -1
 *   trailing non-binary within the window stops the parse (OK if ≥1 digit)
 *   leading '+' / '-' / whitespace / "0b" prefix not accepted
 *
 * Examples (n large enough):
 *   "1010x" → 10, return 4
 *   "0"     → 0, return 1
 *   "11" with n=1 → 1, return 1
 */
int
gj_parse_u64_bin_n(const char *szIn, size_t cMax, uint64_t *pOut)
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
	if (iPos >= cMax || (*pCur != '0' && *pCur != '1')) {
		return -1;
	}

	uVal = 0ull;
	nDig = 0;
	while (iPos < cMax && (*pCur == '0' || *pCur == '1')) {
		uint64_t uBit = (uint64_t)(*pCur - '0');

		if (uVal > (UINT64_MAX >> 1)) {
			return -1;
		}
		uVal = (uVal << 1) | uBit;
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

int __gj_parse_u64_bin_n(const char *szIn, size_t cMax, uint64_t *pOut)
    __attribute__((alias("gj_parse_u64_bin_n")));
