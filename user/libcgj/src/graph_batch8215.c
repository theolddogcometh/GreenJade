/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8215: ctype-class isspace predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ctype_isspace_u_8215(uint32_t ch);
 *     - Return 1 if ch is an ASCII white-space class member; else 0.
 *   int __gj_ctype_isspace_u_8215  (alias)
 *   __libcgj_batch8215_marker = "libcgj-batch8215"
 *
 * Ctype class stubs wave (8211-8220). Distinct from isspace / isspace_l
 * — unique _u_8215 surface; no multi-def. No parent wires.
 *
 * Class members: ' ' (SP), '\t' (HT), '\n' (LF), '\r' (CR),
 * '\f' (FF), '\v' (VT).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8215_marker[] = "libcgj-batch8215";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is ASCII white-space. */
static int
b8215_isspace(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)' ' || u32Ch == (uint32_t)'\t' ||
	    u32Ch == (uint32_t)'\n' || u32Ch == (uint32_t)'\r' ||
	    u32Ch == (uint32_t)'\f' || u32Ch == (uint32_t)'\v') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ctype_isspace_u_8215 - 1 iff ch is ASCII white-space.
 *
 * Edge cases:
 *   ' ' / '\t' / '\n' / '\r' / '\f' / '\v' → 1
 *   '0' / 'A' / any other → 0
 */
int
gj_ctype_isspace_u_8215(uint32_t u32Ch)
{
	(void)NULL;
	return b8215_isspace(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ctype_isspace_u_8215(uint32_t u32Ch)
    __attribute__((alias("gj_ctype_isspace_u_8215")));
