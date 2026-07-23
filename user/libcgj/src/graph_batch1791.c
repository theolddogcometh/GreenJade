/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1791: semver leading major parser.
 *
 * Surface (unique symbols):
 *   int gj_semver_parse_maj(const char *s, unsigned *out);
 *     — Parse a non-empty run of decimal digits at the start of s as
 *       the major version (leading component of "x.y.z") into *out.
 *       Returns 0 on success, -1 on failure (NULL s/out, no leading
 *       digit, or overflow). Stops at first non-digit ('.', '-', '+',
 *       NUL, …). Leading zeros accepted as plain decimal ("01" → 1).
 *   int __gj_semver_parse_maj  (alias)
 *   __libcgj_batch1791_marker = "libcgj-batch1791"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1791_marker[] = "libcgj-batch1791";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is ASCII decimal digit. */
static int
b1791_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/*
 * Parse a non-empty decimal run at *pp into *pOut; advance *pp past digits.
 * Returns 0 on success, -1 on empty run or overflow.
 */
static int
b1791_parse_u(const char **pp, unsigned *pOut)
{
	const char *p;
	unsigned uVal;
	int fAny;

	p = *pp;
	if (!b1791_is_digit(*p)) {
		return -1;
	}
	uVal = 0u;
	fAny = 0;
	while (b1791_is_digit(*p)) {
		unsigned uDigit;
		unsigned uNext;

		uDigit = (unsigned)(*p - '0');
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
	*pp = p;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_semver_parse_maj — parse leading major into *out; 0 / -1.
 */
int
gj_semver_parse_maj(const char *sz, unsigned *pOut)
{
	const char *p;
	unsigned uVal;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}
	p = sz;
	if (b1791_parse_u(&p, &uVal) != 0) {
		return -1;
	}
	*pOut = uVal;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_semver_parse_maj(const char *sz, unsigned *pOut)
    __attribute__((alias("gj_semver_parse_maj")));
