/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1076: freestanding signed decimal → int32_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_i32(const char *s, int32_t *out);
 *     — Parse full-string ASCII decimal with optional leading +/– into
 *       *out. Success → 0; failure → -1 (no write).
 *   int __gj_parse_i32  (alias)
 *   __libcgj_batch1076_marker = "libcgj-batch1076"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1076_marker[] = "libcgj-batch1076";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_i32 — full-string optional-signed decimal ASCII → int32_t.
 *
 * Grammar: [+|-]? [0-9]+
 * Overflow past INT32 range → -1.
 */
int
gj_parse_i32(const char *sz, int32_t *pOut)
{
	const char *p;
	uint32_t uMag;
	int nDig;
	int fNeg;
	uint32_t uLimit;

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
			*pOut = INT32_MIN;
		} else {
			*pOut = -(int32_t)uMag;
		}
	} else {
		*pOut = (int32_t)uMag;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_i32(const char *sz, int32_t *pOut)
    __attribute__((alias("gj_parse_i32")));
