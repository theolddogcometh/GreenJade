/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8212: ctype-class isdigit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isdigit_u_8212(uint32_t ch);
 *     - Return 1 if ch is an ASCII decimal digit '0'..'9'; else 0.
 *   int __gj_ctype_isdigit_u_8212  (alias)
 *   __libcgj_batch8212_marker = "libcgj-batch8212"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isdigit / isdigit_l
 * and gj_ascii_isdigit_u — unique _u_8212 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8212_marker[] = "libcgj-batch8212";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in '0'..'9'. */
static int
b8212_isdigit(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isdigit_u_8212 - 1 iff ch is an ASCII decimal digit.
 *
 * Edge cases:
 *   '0'..'9' → 1
 *   'a' / 'A' / ' ' / any other → 0
 */
int
gj_ctype_isdigit_u_8212(uint32_t u32Ch)
{
	(void)NULL;
	return b8212_isdigit(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isdigit_u_8212(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isdigit_u_8212")));
