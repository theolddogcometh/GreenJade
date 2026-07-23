/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8216: ctype-class isupper predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isupper_u_8216(uint32_t ch);
 *     - Return 1 if ch is an ASCII uppercase letter 'A'..'Z'; else 0.
 *   int __gj_ctype_isupper_u_8216  (alias)
 *   __libcgj_batch8216_marker = "libcgj-batch8216"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isupper / isupper_l
 * — unique _u_8216 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8216_marker[] = "libcgj-batch8216";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 'A'..'Z'. */
static int
b8216_isupper(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'A' && u32Ch <= (uint32_t)'Z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isupper_u_8216 - 1 iff ch is an ASCII uppercase letter.
 *
 * Edge cases:
 *   'A'..'Z' → 1
 *   'a'..'z' / '0'..'9' / any other → 0
 */
int
gj_ctype_isupper_u_8216(uint32_t u32Ch)
{
	(void)NULL;
	return b8216_isupper(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isupper_u_8216(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isupper_u_8216")));
