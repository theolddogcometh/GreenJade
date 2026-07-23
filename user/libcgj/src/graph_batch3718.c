/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3718: full-string signed decimal → int32_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_parse_i32_dec_u(const char *s, int32_t *out);
 *     - Parse a full-string ASCII decimal integer with optional leading
 *       '+' or '-' into *out. On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_i32_dec_u  (alias)
 *   __libcgj_batch3718_marker = "libcgj-batch3718"
 *
 * Exclusive continuum CREATE-ONLY (3711-3720). Distinct from
 * gj_parse_i32 (batch1076), gj_parse_i32_dec_n (batch2143 bounded) —
 * unique _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3718_marker[] = "libcgj-batch3718";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i32_dec_u — full-string optional-signed decimal ASCII → int32_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [+|-]? [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-digit junk / overflow past int32 range → -1
 *   "0", "+0", "-0", "00" → 0
 *   whitespace not accepted
 *   INT32_MIN / INT32_MAX accepted; one past either → -1
 *
 * Examples:
 *   "42" / "+42" → 42
 *   "-1"         → -1
 *   "2147483647"  → INT32_MAX
 *   "-2147483648" → INT32_MIN
 *   "" / "x" / "1x" / " 1" → -1
 */
int
gj_parse_i32_dec_u(const char *s, int32_t *out)
{
	const char *p;
	uint32_t uMag;
	int nDig;
	int fNeg;
	uint32_t uLimit;

	(void)NULL;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;
	fNeg = 0;
	if (*p == '+' || *p == '-') {
		if (*p == '-') {
			fNeg = 1;
		}
		p++;
	}

	if (*p < '0' || *p > '9') {
		return -1;
	}

	/* Magnitude limit: INT32_MAX, or INT32_MIN magnitude when negative. */
	uLimit = (uint32_t)INT32_MAX;
	if (fNeg) {
		uLimit = (uint32_t)INT32_MAX + 1u;
	}

	uMag = 0u;
	nDig = 0;
	while (*p >= '0' && *p <= '9') {
		uint32_t uDig = (uint32_t)(*p - '0');

		if (uMag > (uLimit - uDig) / 10u) {
			return -1;
		}
		uMag = uMag * 10u + uDig;
		p++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}
	if (*p != '\0') {
		return -1;
	}

	if (fNeg) {
		if (uMag == (uint32_t)INT32_MAX + 1u) {
			*out = INT32_MIN;
		} else {
			*out = -(int32_t)uMag;
		}
	} else {
		*out = (int32_t)uMag;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_i32_dec_u(const char *s, int32_t *out)
    __attribute__((alias("gj_parse_i32_dec_u")));
