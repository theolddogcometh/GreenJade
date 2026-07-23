/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1792: semver minor component parser.
 *
 * Surface (unique symbols):
 *   int gj_semver_parse_minor(const char *s, unsigned *out);
 *     — Parse the minor version from a leading "x.y…" form: skip major
 *       digits, require '.', parse the next decimal run into *out.
 *       Returns 0 on success, -1 on failure (NULL, missing major/dot,
 *       no minor digits, overflow). Does not require a trailing patch.
 *   int __gj_semver_parse_minor  (alias)
 *   __libcgj_batch1792_marker = "libcgj-batch1792"
 *
 * Version/semver exclusive wave (1791–1799). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1792_marker[] = "libcgj-batch1792";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1792_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/* Skip a non-empty digit run; returns 0 ok, -1 if no digits. */
static int
b1792_skip_digits(const char **pp)
{
	const char *p;
	int fAny;

	p = *pp;
	fAny = 0;
	while (b1792_is_digit(*p)) {
		fAny = 1;
		p++;
	}
	if (!fAny) {
		return -1;
	}
	*pp = p;
	return 0;
}

/* Parse non-empty decimal run at *pp into *pOut; advance *pp. 0 / -1. */
static int
b1792_parse_u(const char **pp, unsigned *pOut)
{
	const char *p;
	unsigned uVal;
	int fAny;

	p = *pp;
	if (!b1792_is_digit(*p)) {
		return -1;
	}
	uVal = 0u;
	fAny = 0;
	while (b1792_is_digit(*p)) {
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
 * gj_semver_parse_minor — parse minor after "major." into *out; 0 / -1.
 */
int
gj_semver_parse_minor(const char *sz, unsigned *pOut)
{
	const char *p;
	unsigned uVal;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}
	p = sz;
	if (b1792_skip_digits(&p) != 0) {
		return -1;
	}
	if (*p != '.') {
		return -1;
	}
	p++;
	if (b1792_parse_u(&p, &uVal) != 0) {
		return -1;
	}
	*pOut = uVal;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_semver_parse_minor(const char *sz, unsigned *pOut)
    __attribute__((alias("gj_semver_parse_minor")));
