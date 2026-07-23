/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4288: ASCII printable predicate (_u,
 * unsigned code unit).
 *
 * Surface (unique symbols):
 *   int gj_ascii_isprint_u2(unsigned c);
 *     - Return 1 if c is an ASCII printable (0x20..0x7E); else 0.
 *   int __gj_ascii_isprint_u2  (alias)
 *   __libcgj_batch4288_marker = "libcgj-batch4288"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from isprint (ctype.c)
 * and gj_ascii_is_print_n (span, batch661). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4288_marker[] = "libcgj-batch4288";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff uC is in 0x20..0x7E. */
static int
b4288_isprint(unsigned uC)
{
	if (uC >= 0x20u && uC <= 0x7Eu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isprint_u2 - 1 iff c is an ASCII printable code unit.
 *
 * c: unsigned code unit (typically a promoted octet; wider values → 0)
 *
 * Edge cases:
 *   0x20..0x7E (space through '~') → 1
 *   control / DEL / any other → 0
 */
int
gj_ascii_isprint_u2(unsigned uC)
{
	(void)NULL;
	return b4288_isprint(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isprint_u2(unsigned uC)
    __attribute__((alias("gj_ascii_isprint_u2")));
