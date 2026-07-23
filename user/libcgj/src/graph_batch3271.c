/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3271: ASCII uppercase single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_isupper_u(unsigned char c);
 *     - Return 1 if c is in 'A'..'Z' (0x41..0x5A); else 0.
 *   int __gj_ascii_isupper_u  (alias)
 *   __libcgj_batch3271_marker = "libcgj-batch3271"
 *
 * ASCII exclusive wave (3271-3280). Distinct from isupper / isupper_l
 * and gj_ascii_isalpha_u (batch2819) — unique single-octet _u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3271_marker[] = "libcgj-batch3271";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII uppercase A..Z. */
static int
b3271_isupper(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_isupper_u - true when c is an ASCII uppercase letter.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for 'A'..'Z', else 0. Does not call libc isupper.
 */
int
gj_ascii_isupper_u(unsigned char u8C)
{
	(void)NULL;
	return b3271_isupper(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_isupper_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_isupper_u")));
