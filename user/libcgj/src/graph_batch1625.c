/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1625: ASCII isupper predicate on a C string.
 *
 * Surface (unique symbols):
 *   int gj_str_is_upper_ascii(const char *s);
 *     — Return 1 if every cased ASCII letter in s is uppercase and at
 *       least one such letter exists; else 0. Non-letters ignored for
 *       the case rule. NULL / empty / no letters → 0.
 *   int __gj_str_is_upper_ascii  (alias)
 *   __libcgj_batch1625_marker = "libcgj-batch1625"
 *
 * Distinct from ctype isupper (single unit, locale graph). Unique gj_
 * surface only — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1625_marker[] = "libcgj-batch1625";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1625_is_lower(unsigned char u8C)
{
	return (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ? 1 : 0;
}

static int
b1625_is_upper(unsigned char u8C)
{
	return (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_upper_ascii — 1 if s is "uppercase" (ASCII letter rule).
 *
 * All a–z must be absent; at least one A–Z must be present. Digits,
 * punctuation, and other octets are allowed and do not affect the
 * cased-letter requirement.
 */
int
gj_str_is_upper_ascii(const char *s)
{
	const unsigned char *p;
	int fAnyUpper;

	if (s == NULL || s[0] == '\0') {
		return 0;
	}

	fAnyUpper = 0;
	p = (const unsigned char *)s;
	while (*p != 0u) {
		if (b1625_is_lower(*p)) {
			return 0;
		}
		if (b1625_is_upper(*p)) {
			fAnyUpper = 1;
		}
		p++;
	}
	return fAnyUpper;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_upper_ascii(const char *s)
    __attribute__((alias("gj_str_is_upper_ascii")));
