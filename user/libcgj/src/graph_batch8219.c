/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8219: ctype-class isprint predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isprint_u_8219(uint32_t ch);
 *     - Return 1 if ch is a printable ASCII code unit 0x20..0x7e; else 0.
 *   int __gj_ctype_isprint_u_8219  (alias)
 *   __libcgj_batch8219_marker = "libcgj-batch8219"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isprint / isprint_l
 * — unique _u_8219 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8219_marker[] = "libcgj-batch8219";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 0x20..0x7e (printable ASCII). */
static int
b8219_isprint(uint32_t u32Ch)
{
	if (u32Ch >= 0x20u && u32Ch <= 0x7eu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isprint_u_8219 - 1 iff ch is printable ASCII (SP..~).
 *
 * Edge cases:
 *   0x20..0x7e → 1
 *   0x00..0x1f / 0x7f / 0x80.. → 0
 */
int
gj_ctype_isprint_u_8219(uint32_t u32Ch)
{
	(void)NULL;
	return b8219_isprint(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isprint_u_8219(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isprint_u_8219")));
