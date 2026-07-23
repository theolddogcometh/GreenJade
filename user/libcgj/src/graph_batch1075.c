/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1075: freestanding signed decimal → int64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_i64(const char *s, int64_t *out);
 *     — Parse full-string ASCII decimal with optional leading +/– into
 *       *out. Success → 0; failure → -1 (no write).
 *   int __gj_parse_i64  (alias)
 *   __libcgj_batch1075_marker = "libcgj-batch1075"
 *
 * Distinct from gj_parse_i64_dec (batch446) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1075_marker[] = "libcgj-batch1075";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i64 — full-string optional-signed decimal ASCII → int64_t.
 *
 * Grammar: [+|-]? [0-9]+
 * Whitespace / trailing junk / overflow → -1.
 */
int
gj_parse_i64(const char *sz, int64_t *pOut)
{
	const char *p;
	uint64_t uMag;
	int nDig;
	int fNeg;
	uint64_t uLimit;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}

	p = sz;
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

int __gj_parse_i64(const char *sz, int64_t *pOut)
    __attribute__((alias("gj_parse_i64")));
