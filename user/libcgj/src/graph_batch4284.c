/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4284: ASCII alphanumeric predicate (_u,
 * unsigned code unit).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isalnum_u2(unsigned c);
 *     - Return 1 if c is an ASCII letter or decimal digit; else 0.
 *   int __gj_ascii_isalnum_u2  (alias)
 *   __libcgj_batch4284_marker = "libcgj-batch4284"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from isalnum (ctype.c)
 * and gj_ascii_is_alnum_n (span, batch659). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4284_marker[] = "libcgj-batch4284";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uC is 0-9 / a-z / A-Z. */
static int
b4284_isalnum(unsigned uC)
{
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return 1;
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'z') {
		return 1;
	}
	if (uC >= (unsigned)'A' && uC <= (unsigned)'Z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isalnum_u2 - 1 iff c is an ASCII alphanumeric code unit.
 *
 * c: unsigned code unit (typically a promoted octet; wider values → 0)
 *
 * Edge cases:
 *   '0'..'9' / 'a'..'z' / 'A'..'Z' → 1
 *   ' ' / punctuation / any other → 0
 */
int
gj_ascii_isalnum_u2(unsigned uC)
{
	(void)NULL;
	return b4284_isalnum(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isalnum_u2(unsigned uC)
    __attribute__((alias("gj_ascii_isalnum_u2")));
