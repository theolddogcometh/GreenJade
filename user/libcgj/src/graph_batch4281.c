/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4281: ASCII decimal-digit predicate (_u,
 * unsigned code unit).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isdigit_u2(unsigned c);
 *     - Return 1 if c is an ASCII decimal digit '0'..'9'; else 0.
 *   int __gj_ascii_isdigit_u2  (alias)
 *   __libcgj_batch4281_marker = "libcgj-batch4281"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from isdigit (ctype.c)
 * and gj_ascii_is_digit_n (span, batch657). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4281_marker[] = "libcgj-batch4281";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uC is in '0'..'9'. */
static int
b4281_isdigit(unsigned uC)
{
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isdigit_u2 - 1 iff c is an ASCII decimal digit.
 *
 * c: unsigned code unit (typically a promoted octet; wider values → 0)
 *
 * Edge cases:
 *   '0'..'9' → 1
 *   'a' / 'A' / ' ' / any other → 0
 */
int
gj_ascii_isdigit_u2(unsigned uC)
{
	(void)NULL;
	return b4281_isdigit(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isdigit_u2(unsigned uC)
    __attribute__((alias("gj_ascii_isdigit_u2")));
