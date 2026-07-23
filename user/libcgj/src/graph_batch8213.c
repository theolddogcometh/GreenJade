/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8213: ctype-class isalpha predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isalpha_u_8213(uint32_t ch);
 *     - Return 1 if ch is an ASCII letter 'A'..'Z' or 'a'..'z'; else 0.
 *   int __gj_ctype_isalpha_u_8213  (alias)
 *   __libcgj_batch8213_marker = "libcgj-batch8213"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isalpha / isalpha_l
 * — unique _u_8213 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8213_marker[] = "libcgj-batch8213";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 'A'..'Z' or 'a'..'z'. */
static int
b8213_isalpha(uint32_t u32Ch)
{
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
 * gj_ctype_isalpha_u_8213 - 1 iff ch is an ASCII alphabetic letter.
 *
 * Edge cases:
 *   'A'..'Z' / 'a'..'z' → 1
 *   '0'..'9' / ' ' / any other → 0
 */
int
gj_ctype_isalpha_u_8213(uint32_t u32Ch)
{
	(void)NULL;
	return b8213_isalpha(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isalpha_u_8213(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isalpha_u_8213")));
