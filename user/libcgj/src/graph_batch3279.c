/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3279: ASCII case-insensitive octet compare.
 *
 * Surface (unique symbols):
 *   int gj_ascii_casecmp_u(unsigned char a, unsigned char b);
 *     - Fold A..Z to a..z on both sides, then return (int)fa - (int)fb
 *       (strcmp-style: negative / zero / positive).
 *   int __gj_ascii_casecmp_u  (alias)
 *   __libcgj_batch3279_marker = "libcgj-batch3279"
 *
 * ASCII exclusive wave (3271-3280). Distinct from strcasecmp /
 * memcasecmp and scalar case-fold helpers — unique two-octet _u
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3279_marker[] = "libcgj-batch3279";

/* ---- freestanding helpers ---------------------------------------------- */

/* Fold one ASCII uppercase letter to lowercase; else identity. */
static unsigned char
b3279_fold(unsigned char u8C)
{
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') {
		return (unsigned char)(u8C + (unsigned char)('a' - 'A'));
	}
	return u8C;
}

/* strcmp-style compare after ASCII case fold. */
static int
b3279_casecmp(unsigned char u8A, unsigned char u8B)
{
	unsigned char u8Fa = b3279_fold(u8A);
	unsigned char u8Fb = b3279_fold(u8B);

	return (int)u8Fa - (int)u8Fb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_casecmp_u - compare two octets case-insensitively (ASCII).
 *
 * a, b: candidate octets (0..255)
 *
 * Only A..Z fold to a..z; all other octets compare as themselves.
 * Returns negative if folded(a) < folded(b), zero if equal, positive
 * if greater. Does not call libc strcasecmp.
 */
int
gj_ascii_casecmp_u(unsigned char u8A, unsigned char u8B)
{
	(void)NULL;
	return b3279_casecmp(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_casecmp_u(unsigned char u8A, unsigned char u8B)
    __attribute__((alias("gj_ascii_casecmp_u")));
