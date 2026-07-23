/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3272: ASCII lowercase single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_islower_u(unsigned char c);
 *     - Return 1 if c is in 'a'..'z' (0x61..0x7A); else 0.
 *   int __gj_ascii_islower_u  (alias)
 *   __libcgj_batch3272_marker = "libcgj-batch3272"
 *
 * ASCII exclusive wave (3271-3280). Distinct from islower / islower_l
 * and gj_ascii_isalpha_u (batch2819) — unique single-octet _u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3272_marker[] = "libcgj-batch3272";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII lowercase a..z. */
static int
b3272_islower(unsigned char u8C)
{
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_islower_u - true when c is an ASCII lowercase letter.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for 'a'..'z', else 0. Does not call libc islower.
 */
int
gj_ascii_islower_u(unsigned char u8C)
{
	(void)NULL;
	return b3272_islower(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_islower_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_islower_u")));
