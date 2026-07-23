/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1864: Proton version leading-major parser.
 *
 * Surface (unique symbols):
 *   int gj_proton_major_from_ver(const char *s, unsigned *out);
 *     — Parse leading major from a "Proton X.Y" style string into *out.
 *       Accepts optional "Proton" / "proton" prefix and whitespace, then
 *       a non-empty decimal run (major). Stops at first non-digit ('.',
 *       NUL, …). Returns 0 on success, -1 on failure (NULL, no digits,
 *       overflow). Does not require a minor component.
 *   int __gj_proton_major_from_ver  (alias)
 *   __libcgj_batch1864_marker = "libcgj-batch1864"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Distinct from gj_semver_parse_maj — unique Proton surface. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1864_marker[] = "libcgj-batch1864";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1864_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

static int
b1864_is_space(char ch)
{
	return (ch == ' ' || ch == '\t') ? 1 : 0;
}

/* Case-insensitive match of ASCII letter. */
static int
b1864_eq_ci(char ch, char chLower)
{
	char chFold;

	chFold = ch;
	if (chFold >= 'A' && chFold <= 'Z') {
		chFold = (char)(chFold - 'A' + 'a');
	}
	return (chFold == chLower) ? 1 : 0;
}

/*
 * If p begins with "proton" (any case), advance past it; else leave p.
 * Always succeeds (prefix is optional).
 */
static const char *
b1864_skip_proton(const char *p)
{
	const char *q;
	static const char szProton[] = "proton";
	size_t i;

	q = p;
	for (i = 0; szProton[i] != '\0'; i++) {
		if (!b1864_eq_ci(q[i], szProton[i])) {
			return p;
		}
	}
	return q + i;
}

static const char *
b1864_skip_space(const char *p)
{
	while (b1864_is_space(*p)) {
		p++;
	}
	return p;
}

/*
 * Parse a non-empty decimal run at *pp into *pOut; advance *pp past digits.
 * Returns 0 on success, -1 on empty run or overflow.
 */
static int
b1864_parse_u(const char **pp, unsigned *pOut)
{
	const char *p;
	unsigned uVal;
	int fAny;

	p = *pp;
	if (!b1864_is_digit(*p)) {
		return -1;
	}
	uVal = 0u;
	fAny = 0;
	while (b1864_is_digit(*p)) {
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
 * gj_proton_major_from_ver — parse major from "Proton X.Y" into *out.
 *
 * Examples:
 *   "Proton 9.0"   → 0, *out = 9
 *   "proton 8.0-5" → 0, *out = 8
 *   "9.0-GE"       → 0, *out = 9  (prefix optional)
 *   "Proton"       → -1
 *   NULL           → -1
 */
int
gj_proton_major_from_ver(const char *sz, unsigned *pOut)
{
	const char *p;
	unsigned uVal;

	(void)sizeof(NULL);
	if (sz == NULL || pOut == NULL) {
		return -1;
	}
	p = sz;
	p = b1864_skip_space(p);
	p = b1864_skip_proton(p);
	p = b1864_skip_space(p);
	if (b1864_parse_u(&p, &uVal) != 0) {
		return -1;
	}
	*pOut = uVal;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_proton_major_from_ver(const char *sz, unsigned *pOut)
    __attribute__((alias("gj_proton_major_from_ver")));
