/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1176: semver leading major number parser.
 *
 * Surface (unique symbols):
 *   int gj_semver_parse_major(const char *s, unsigned *out);
 *     — Parse a non-empty run of decimal digits at the start of s as
 *       the major version into *out. Returns 0 on success, -1 on
 *       failure (NULL s/out, no leading digit, or overflow).
 *       Stops at first non-digit ('.', '-', '+', NUL, …). Leading
 *       zeros are accepted as plain decimal ("01" → 1).
 *   int __gj_semver_parse_major  (alias)
 *   __libcgj_batch1176_marker = "libcgj-batch1176"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1176_marker[] = "libcgj-batch1176";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_semver_parse_major — parse leading major into *out; 0 / -1.
 */
int
gj_semver_parse_major(const char *sz, unsigned *pOut)
{
	const char *p;
	unsigned uVal;
	int fAny;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}
	p = sz;
	if (*p < '0' || *p > '9') {
		return -1;
	}
	uVal = 0u;
	fAny = 0;
	while (*p >= '0' && *p <= '9') {
		unsigned uDigit;
		unsigned uNext;

		uDigit = (unsigned)(*p - '0');
		/* Overflow guard: uVal * 10 + digit */
		if (uVal > (0xffffffffu / 10u)) {
			return -1;
		}
		uNext = uVal * 10u;
		if (uNext > (0xffffffffu - uDigit)) {
			return -1;
		}
		uVal = uNext + uDigit;
		fAny = 1;
		p++;
	}
	if (!fAny) {
		return -1;
	}
	*pOut = uVal;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_semver_parse_major(const char *sz, unsigned *pOut)
    __attribute__((alias("gj_semver_parse_major")));
