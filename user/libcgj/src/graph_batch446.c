/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch446: freestanding signed decimal string → int64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_i64_dec(const char *s, int64_t *out);
 *     — Parse a full-string ASCII decimal integer with optional leading
 *       '+' or '-' into *out. On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_i64_dec  (alias)
 *   __libcgj_batch446_marker = "libcgj-batch446"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch446_marker[] = "libcgj-batch446";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i64_dec — full-string optional-signed decimal ASCII → int64_t.
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
gj_parse_i64_dec(const char *s, int64_t *out)
{
	const char *p;
	uint64_t uMag;
	int nDig;
	int fNeg;
	uint64_t uLimit;

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

	/* Magnitude limit: INT64_MAX, or INT64_MIN magnitude when negative. */
	uLimit = (uint64_t)INT64_MAX;
	if (fNeg) {
		uLimit = (uint64_t)INT64_MAX + 1ull;
	}

	uMag = 0ull;
	nDig = 0;
	while (*p >= '0' && *p <= '9') {
		uint64_t uDig = (uint64_t)(*p - '0');

		if (uMag > (uLimit - uDig) / 10ull) {
			return -1;
		}
		uMag = uMag * 10ull + uDig;
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
		if (uMag == (uint64_t)INT64_MAX + 1ull) {
			*out = INT64_MIN;
		} else {
			*out = -(int64_t)uMag;
		}
	} else {
		*out = (int64_t)uMag;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_i64_dec(const char *s, int64_t *out)
    __attribute__((alias("gj_parse_i64_dec")));
