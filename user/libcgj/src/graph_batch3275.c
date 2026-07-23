/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3275: ASCII graphic single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_isgraph_u(unsigned char c);
 *     - Return 1 if c is in 0x21..0x7E (printable non-space); else 0.
 *   int __gj_ascii_isgraph_u  (alias)
 *   __libcgj_batch3275_marker = "libcgj-batch3275"
 *
 * ASCII exclusive wave (3271-3280). Distinct from isgraph / isgraph_l
 * and gj_ascii_isprint_u (batch2823, includes space) — unique _u
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3275_marker[] = "libcgj-batch3275";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII graphic 0x21..0x7E. */
static int
b3275_isgraph(unsigned char u8C)
{
	if (u8C >= 0x21u && u8C <= 0x7Eu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isgraph_u - true when c is an ASCII graphic octet.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for 0x21..0x7E inclusive (space 0x20 fails). Does not call
 * libc isgraph.
 */
int
gj_ascii_isgraph_u(unsigned char u8C)
{
	(void)NULL;
	return b3275_isgraph(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isgraph_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isgraph_u")));
