/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8214: ctype-class isalnum predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isalnum_u_8214(uint32_t ch);
 *     - Return 1 if ch is ASCII alphanumeric (letter or digit); else 0.
 *   int __gj_ctype_isalnum_u_8214  (alias)
 *   __libcgj_batch8214_marker = "libcgj-batch8214"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isalnum / isalnum_l
 * — unique _u_8214 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8214_marker[] = "libcgj-batch8214";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is digit or alphabetic ASCII. */
static int
b8214_isalnum(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		return 1;
	}
	if (u32Ch >= (uint32_t)'A' && u32Ch <= (uint32_t)'Z') {
		return 1;
	}
	if (u32Ch >= (uint32_t)'a' && u32Ch <= (uint32_t)'z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isalnum_u_8214 - 1 iff ch is ASCII letter or digit.
 *
 * Edge cases:
 *   '0'..'9' / 'A'..'Z' / 'a'..'z' → 1
 *   ' ' / punctuation / any other → 0
 */
int
gj_ctype_isalnum_u_8214(uint32_t u32Ch)
{
	(void)NULL;
	return b8214_isalnum(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isalnum_u_8214(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isalnum_u_8214")));
