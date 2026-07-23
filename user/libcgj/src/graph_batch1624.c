/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1624: ASCII islower predicate on a C string.
 *
 * Surface (unique symbols):
 *   int gj_str_is_lower_ascii(const char *s);
 *     — Return 1 if every cased ASCII letter in s is lowercase and at
 *       least one such letter exists; else 0. Non-letters ignored for
 *       the case rule. NULL / empty / no letters → 0.
 *   int __gj_str_is_lower_ascii  (alias)
 *   __libcgj_batch1624_marker = "libcgj-batch1624"
 *
 * Distinct from ctype islower (single unit, locale graph). Unique gj_
 * surface only — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1624_marker[] = "libcgj-batch1624";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1624_is_lower(unsigned char u8C)
{
	return (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ? 1 : 0;
}

static int
b1624_is_upper(unsigned char u8C)
{
	return (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_lower_ascii — 1 if s is "lowercase" (ASCII letter rule).
 *
 * All A–Z must be absent; at least one a–z must be present. Digits,
 * punctuation, and other octets are allowed and do not affect the
 * cased-letter requirement.
 */
int
gj_str_is_lower_ascii(const char *s)
{
	const unsigned char *p;
	int fAnyLower;

	if (s == NULL || s[0] == '\0') {
		return 0;
	}

	fAnyLower = 0;
	p = (const unsigned char *)s;
	while (*p != 0u) {
		if (b1624_is_upper(*p)) {
			return 0;
		}
		if (b1624_is_lower(*p)) {
			fAnyLower = 1;
		}
		p++;
	}
	return fAnyLower;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_lower_ascii(const char *s)
    __attribute__((alias("gj_str_is_lower_ascii")));
