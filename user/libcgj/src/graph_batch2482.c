/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2482: full-string signed decimal → int64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_i64_dec_full(const char *s, int64_t *out);
 *     — Parse a full-string ASCII decimal integer with optional leading
 *       '+' or '-' into *out. On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_i64_dec_full  (alias)
 *   __libcgj_batch2482_marker = "libcgj-batch2482"
 *
 * Distinct from gj_parse_i64_dec (batch446). Post-2480 parse exclusive
 * wave (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2482_marker[] = "libcgj-batch2482";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2482_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i64_dec_full — full-string optional-signed decimal → int64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [+|-]? [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-digit junk / overflow past int64 range → -1
 *   "0", "+0", "-0", "00" → 0
 *   whitespace not accepted
 *   INT64_MIN / INT64_MAX accepted; one past either → -1
 *
 * Examples:
 *   "42" / "+42" → 42
 *   "-1"         → -1
 *   "9223372036854775807"  → INT64_MAX
 *   "-9223372036854775808" → INT64_MIN
 *   "" / "x" / "1x" / " 1" → -1
 */
int
gj_parse_i64_dec_full(const char *sz, int64_t *pOut)
{
	const char *pCur;
	uint64_t uMag;
	int nDig;
	int fNeg;
	uint64_t uLimit;

	(void)NULL;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}

	pCur = sz;
	fNeg = 0;
	if (*pCur == '+' || *pCur == '-') {
		if (*pCur == '-') {
			fNeg = 1;
		}
		pCur++;
	}

	if (!b2482_is_digit(*pCur)) {
		return -1;
	}

	/* Magnitude limit: INT64_MAX, or INT64_MIN magnitude when negative. */
	uLimit = (uint64_t)INT64_MAX;
	if (fNeg) {
		uLimit = (uint64_t)INT64_MAX + 1ull;
	}

	uMag = 0ull;
	nDig = 0;
	while (b2482_is_digit(*pCur)) {
		uint64_t uDig = (uint64_t)(*pCur - '0');

		if (uMag > (uLimit - uDig) / 10ull) {
			return -1;
		}
		uMag = uMag * 10ull + uDig;
		pCur++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}
	if (*pCur != '\0') {
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
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_i64_dec_full(const char *sz, int64_t *pOut)
    __attribute__((alias("gj_parse_i64_dec_full")));
