/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3273: ASCII control single-octet predicate.
 *
 * Surface (unique symbols):
 *   int gj_ascii_iscntrl_u(unsigned char c);
 *     - Return 1 if c is in 0x00..0x1F or is DEL (0x7F); else 0.
 *   int __gj_ascii_iscntrl_u  (alias)
 *   __libcgj_batch3273_marker = "libcgj-batch3273"
 *
 * ASCII exclusive wave (3271-3280). Distinct from iscntrl / iscntrl_l —
 * unique single-octet _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3273_marker[] = "libcgj-batch3273";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u8C is ASCII C0 control or DEL. */
static int
b3273_iscntrl(unsigned char u8C)
{
	if (u8C <= 0x1Fu) {
		return 1;
	}
	if (u8C == 0x7Fu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_iscntrl_u - true when c is an ASCII control octet.
 *
 * c: candidate octet (0..255)
 *
 * Returns 1 for 0x00..0x1F and 0x7F; high-bit and printable → 0.
 * Does not call libc iscntrl.
 */
int
gj_ascii_iscntrl_u(unsigned char u8C)
{
	(void)NULL;
	return b3273_iscntrl(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_iscntrl_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_iscntrl_u")));
