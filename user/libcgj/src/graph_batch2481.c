/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2481: full-string decimal string → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64_dec_full(const char *s, uint64_t *out);
 *     — Parse a full-string ASCII decimal integer into *out.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_u64_dec_full  (alias)
 *   __libcgj_batch2481_marker = "libcgj-batch2481"
 *
 * Distinct from gj_parse_u64_dec (batch390) and gj_parse_u64_dec_n
 * (batch1531). Post-2480 parse exclusive wave (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2481_marker[] = "libcgj-batch2481";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2481_is_digit — 1 if ch is ASCII '0'..'9', else 0.
 */
static int
b2481_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64_dec_full — full-string decimal ASCII → uint64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-digit junk / overflow past UINT64_MAX → -1
 *   "0", "00", "000" → 0
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"                    → 0
 *   "42"                   → 42
 *   "18446744073709551615" → UINT64_MAX
 *   "18446744073709551616" → -1 (overflow)
 *   "" / "x" / "1x" / " 1" → -1
 */
int
gj_parse_u64_dec_full(const char *sz, uint64_t *pOut)
{
	const char *pCur;
	uint64_t uVal;
	int nDig;

	(void)NULL;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}

	pCur = sz;
	if (!b2481_is_digit(*pCur)) {
		return -1;
	}

	uVal = 0ull;
	nDig = 0;
	while (b2481_is_digit(*pCur)) {
		uint64_t uDig = (uint64_t)(*pCur - '0');

		if (uVal > (UINT64_MAX - uDig) / 10ull) {
			return -1;
		}
		uVal = uVal * 10ull + uDig;
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

int __gj_parse_u64_dec_full(const char *sz, uint64_t *pOut)
    __attribute__((alias("gj_parse_u64_dec_full")));
