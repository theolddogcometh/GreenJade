/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4283: ASCII alphabetic predicate (_u,
 * unsigned code unit).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isalpha_u2(unsigned c);
 *     - Return 1 if c is an ASCII letter a-z / A-Z; else 0.
 *   int __gj_ascii_isalpha_u2  (alias)
 *   __libcgj_batch4283_marker = "libcgj-batch4283"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from isalpha (ctype.c)
 * and gj_ascii_is_alpha_n (span, batch658). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4283_marker[] = "libcgj-batch4283";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uC is a-z or A-Z. */
static int
b4283_isalpha(unsigned uC)
{
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
 * gj_ascii_isalpha_u2 - 1 iff c is an ASCII alphabetic letter.
 *
 * c: unsigned code unit (typically a promoted octet; wider values → 0)
 *
 * Edge cases:
 *   'a'..'z' / 'A'..'Z' → 1
 *   '0'..'9' / ' ' / any other → 0
 */
int
gj_ascii_isalpha_u2(unsigned uC)
{
	(void)NULL;
	return b4283_isalpha(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isalpha_u2(unsigned uC)
    __attribute__((alias("gj_ascii_isalpha_u2")));
