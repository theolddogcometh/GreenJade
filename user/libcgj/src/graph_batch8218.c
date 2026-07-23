/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8218: ctype-class isxdigit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isxdigit_u_8218(uint32_t ch);
 *     - Return 1 if ch is an ASCII hex digit 0-9/a-f/A-F; else 0.
 *   int __gj_ctype_isxdigit_u_8218  (alias)
 *   __libcgj_batch8218_marker = "libcgj-batch8218"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isxdigit /
 * isxdigit_l — unique _u_8218 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8218_marker[] = "libcgj-batch8218";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is hex digit 0-9 / a-f / A-F. */
static int
b8218_isxdigit(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		return 1;
	}
	if (u32Ch >= (uint32_t)'a' && u32Ch <= (uint32_t)'f') {
		return 1;
	}
	if (u32Ch >= (uint32_t)'A' && u32Ch <= (uint32_t)'F') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isxdigit_u_8218 - 1 iff ch is an ASCII hexadecimal digit.
 *
 * Edge cases:
 *   '0'..'9' / 'a'..'f' / 'A'..'F' → 1
 *   'g' / 'G' / ' ' / any other → 0
 */
int
gj_ctype_isxdigit_u_8218(uint32_t u32Ch)
{
	(void)NULL;
	return b8218_isxdigit(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isxdigit_u_8218(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isxdigit_u_8218")));
