/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8217: ctype-class islower predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_islower_u_8217(uint32_t ch);
 *     - Return 1 if ch is an ASCII lowercase letter 'a'..'z'; else 0.
 *   int __gj_ctype_islower_u_8217  (alias)
 *   __libcgj_batch8217_marker = "libcgj-batch8217"
 *
 * Ctype class stubs wave (8211-8220). Distinct from islower / islower_l
 * — unique _u_8217 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8217_marker[] = "libcgj-batch8217";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 'a'..'z'. */
static int
b8217_islower(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'a' && u32Ch <= (uint32_t)'z') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_islower_u_8217 - 1 iff ch is an ASCII lowercase letter.
 *
 * Edge cases:
 *   'a'..'z' → 1
 *   'A'..'Z' / '0'..'9' / any other → 0
 */
int
gj_ctype_islower_u_8217(uint32_t u32Ch)
{
	(void)NULL;
	return b8217_islower(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_islower_u_8217(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_islower_u_8217")));
