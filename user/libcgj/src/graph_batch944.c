/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch944: title-case heuristic (first upper, rest lower per word).
 *
 * Surface (unique symbols):
 *   int gj_str_is_title_case(const char *s);
 *     — 1 if every word has first letter A-Z and remaining letters a-z
 *       (non-letters allowed anywhere; empty words N/A). Empty/NULL → 0.
 *       Non-letter-only words are accepted. Requires at least one letter
 *       overall in the string.
 *   int __gj_str_is_title_case  (alias)
 *   __libcgj_batch944_marker = "libcgj-batch944"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch944_marker[] = "libcgj-batch944";

/* ---- helpers ----------------------------------------------------------- */

static int
b944_is_space(unsigned char u8C)
{
	return (u8C == ' ' || u8C == '\t' || u8C == '\n' ||
	        u8C == '\r' || u8C == '\f' || u8C == '\v');
}

static int
b944_is_upper(unsigned char u8C)
{
	return (u8C >= 'A' && u8C <= 'Z');
}

static int
b944_is_lower(unsigned char u8C)
{
	return (u8C >= 'a' && u8C <= 'z');
}

static int
b944_is_alpha(unsigned char u8C)
{
	return b944_is_upper(u8C) || b944_is_lower(u8C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_title_case — heuristic Title Case check (ASCII letters).
 *
 * Per word (whitespace-separated): the first alphabetic character must
 * be uppercase; subsequent alphabetic characters must be lowercase.
 * Non-letters are ignored for the letter-case rule. Returns 0 if the
 * string has no alphabetic characters at all, or if any word violates
 * the rule.
 */
int
gj_str_is_title_case(const char *pS)
{
	const char *p;
	int fInWord;
	int fSeenFirstAlpha;
	int fAnyAlpha;

	if (pS == NULL || pS[0] == '\0') {
		return 0;
	}

	fInWord = 0;
	fSeenFirstAlpha = 0;
	fAnyAlpha = 0;
	p = pS;
	while (*p != '\0') {
		unsigned char u8C = (unsigned char)*p;

		if (b944_is_space(u8C)) {
			fInWord = 0;
			fSeenFirstAlpha = 0;
		} else {
			if (!fInWord) {
				fInWord = 1;
				fSeenFirstAlpha = 0;
			}
			if (b944_is_alpha(u8C)) {
				fAnyAlpha = 1;
				if (!fSeenFirstAlpha) {
					if (!b944_is_upper(u8C)) {
						return 0;
					}
					fSeenFirstAlpha = 1;
				} else if (!b944_is_lower(u8C)) {
					return 0;
				}
			}
		}
		p++;
	}
	return fAnyAlpha ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_title_case(const char *pS)
    __attribute__((alias("gj_str_is_title_case")));
