/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8211: ctype-class isascii predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isascii_u_8211(uint32_t ch);
 *     - Return 1 if ch is a classic 7-bit ASCII code unit (0..127); else 0.
 *   int __gj_ctype_isascii_u_8211  (alias)
 *   __libcgj_batch8211_marker = "libcgj-batch8211"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isascii / __isascii
 * and gj_wctype_is_ascii — unique _u_8211 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8211_marker[] = "libcgj-batch8211";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 0..127. */
static int
b8211_isascii(uint32_t u32Ch)
{
	if (u32Ch <= 127u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isascii_u_8211 - 1 iff ch is classic 7-bit ASCII.
 *
 * Edge cases:
 *   0..127 → 1
 *   128..UINT32_MAX → 0
 */
int
gj_ctype_isascii_u_8211(uint32_t u32Ch)
{
	(void)NULL;
	return b8211_isascii(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isascii_u_8211(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isascii_u_8211")));
