/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3876: signed integer number string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_number_u(const char *s);
 *     - Return 1 if s matches [+|-]?[0-9]+ fully (non-empty digit run).
 *       Optional leading '+' or '-' must be followed by at least one
 *       ASCII digit. NULL / empty / bare sign / non-digit junk → 0.
 *       No decimal point, exponent, or locale.
 *   int __gj_str_is_number_u  (alias)
 *   __libcgj_batch3876_marker = "libcgj-batch3876"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880). Distinct from
 * gj_str_is_number_ascii (batch2077) — unique gj_str_is_number_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3876_marker[] = "libcgj-batch3876";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3876_is_digit(unsigned char u8C)
{
	return (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') ? 1 : 0;
}

static int
b3876_is_number(const char *sz)
{
	const char *p;
	size_t cDig;

	p = sz;
	if (*p == '+' || *p == '-') {
		p++;
	}

	cDig = 0u;
	while (*p != '\0') {
		if (!b3876_is_digit((unsigned char)*p)) {
			return 0;
		}
		p++;
		cDig++;
	}
	if (cDig == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_number_u - 1 if s is an optional-sign ASCII integer form.
 *
 * Matches [+|-]?[0-9]+ fully. Examples:
 *   "0" / "42" / "+1" / "-07" → 1
 *   NULL / "" / "+" / "-" / "1.0" / "1e3" / " 1" → 0
 * Does not call libc. No parent wires.
 */
int
gj_str_is_number_u(const char *s)
{
	(void)NULL;
	if (s == NULL || s[0] == '\0') {
		return 0;
	}
	return b3876_is_number(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_number_u(const char *s)
    __attribute__((alias("gj_str_is_number_u")));
