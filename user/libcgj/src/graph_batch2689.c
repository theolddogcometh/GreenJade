/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2689: UTF-8 BOM predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf8_bom_p_u(const unsigned char *s, size_t n);
 *     - Return 1 if n >= 3 and s[0..2] is the UTF-8 byte-order mark
 *       EF BB BF. Return 0 otherwise (including NULL s, or n < 3).
 *   int __gj_utf8_bom_p_u  (alias)
 *   __libcgj_batch2689_marker = "libcgj-batch2689"
 *
 * UTF-8 exclusive wave (2681-2690). Unique BOM probe; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2689_marker[] = "libcgj-batch2689";

/* UTF-8 BOM octets. */
#define B2689_BOM0  0xEFu
#define B2689_BOM1  0xBBu
#define B2689_BOM2  0xBFu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff p[0..2] is EF BB BF. p must be non-NULL and n >= 3. */
static int
b2689_bom_p(const unsigned char *p)
{
	if (p[0] != (unsigned char)B2689_BOM0) {
		return 0;
	}
	if (p[1] != (unsigned char)B2689_BOM1) {
		return 0;
	}
	if (p[2] != (unsigned char)B2689_BOM2) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_bom_p_u - true when s begins with the UTF-8 BOM.
 *
 * s: buffer (NULL -> 0)
 * n: available length (must be >= 3 to match)
 *
 * Returns 1 for EF BB BF prefix, else 0. Does not call libc.
 */
int
gj_utf8_bom_p_u(const unsigned char *s, size_t n)
{
	(void)NULL;
	if (s == NULL || n < 3u) {
		return 0;
	}
	return b2689_bom_p(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf8_bom_p_u(const unsigned char *s, size_t n)
    __attribute__((alias("gj_utf8_bom_p_u")));
