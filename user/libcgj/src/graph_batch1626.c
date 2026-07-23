/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1626: ASCII title-case predicate on a C string.
 *
 * Surface (unique symbols):
 *   int gj_str_is_title_ascii(const char *s);
 *     — Return 1 if every whitespace-separated word has first letter
 *       A–Z and subsequent letters a–z (ASCII). Non-letters ignored
 *       for the letter-case rule. Requires at least one letter overall.
 *       NULL / empty / no letters → 0.
 *   int __gj_str_is_title_ascii  (alias)
 *   __libcgj_batch1626_marker = "libcgj-batch1626"
 *
 * Distinct from gj_str_is_title_case (batch944) by unique symbol name.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1626_marker[] = "libcgj-batch1626";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1626_is_space(unsigned char u8C)
{
	return (u8C == (unsigned char)' ' || u8C == (unsigned char)'\t' ||
	    u8C == (unsigned char)'\n' || u8C == (unsigned char)'\r' ||
	    u8C == (unsigned char)'\f' || u8C == (unsigned char)'\v') ? 1 : 0;
}

static int
b1626_is_upper(unsigned char u8C)
{
	return (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ? 1 : 0;
}

static int
b1626_is_lower(unsigned char u8C)
{
	return (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ? 1 : 0;
}

static int
b1626_is_alpha(unsigned char u8C)
{
	return (b1626_is_upper(u8C) || b1626_is_lower(u8C)) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_title_ascii — heuristic Title Case check (ASCII letters).
 *
 * Per word (whitespace-separated): the first alphabetic character must
 * be uppercase; subsequent alphabetic characters must be lowercase.
 * Non-letters are ignored for the letter-case rule. Returns 0 if the
 * string has no alphabetic characters at all, or if any word violates
 * the rule.
 */
int
gj_str_is_title_ascii(const char *s)
{
	const char *p;
	int fInWord;
	int fSeenFirstAlpha;
	int fAnyAlpha;

	if (s == NULL || s[0] == '\0') {
		return 0;
	}

	fInWord = 0;
	fSeenFirstAlpha = 0;
	fAnyAlpha = 0;
	p = s;
	while (*p != '\0') {
		unsigned char u8C = (unsigned char)*p;

		if (b1626_is_space(u8C)) {
			fInWord = 0;
			fSeenFirstAlpha = 0;
		} else {
			if (!fInWord) {
				fInWord = 1;
				fSeenFirstAlpha = 0;
			}
			if (b1626_is_alpha(u8C)) {
				fAnyAlpha = 1;
				if (!fSeenFirstAlpha) {
					if (!b1626_is_upper(u8C)) {
						return 0;
					}
					fSeenFirstAlpha = 1;
				} else if (!b1626_is_lower(u8C)) {
					return 0;
				}
			}
		}
		p++;
	}
	return fAnyAlpha ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_title_ascii(const char *s)
    __attribute__((alias("gj_str_is_title_ascii")));
