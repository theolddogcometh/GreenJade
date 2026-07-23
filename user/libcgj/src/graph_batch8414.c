/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8414: wchar-class isspace predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_wchar_isspace_u_8414(uint32_t ch);
 *     - Return 1 if ch is an ASCII white-space class member; else 0.
 *   int __gj_wchar_isspace_u_8414  (alias)
 *   __libcgj_batch8414_marker = "libcgj-batch8414"
 *
 * Wchar class stubs wave (8411-8420). Distinct from iswspace / isspace
 * and gj_ctype_isspace_u_8215 — unique _u_8414 surface; no multi-def.
 * No parent wires.
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

const char __libcgj_batch8414_marker[] = "libcgj-batch8414";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is ASCII white-space. */
static int
b8414_isspace(uint32_t u32Ch)
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
 * gj_wchar_isspace_u_8414 - 1 iff ch is ASCII white-space.
 *
 * Edge cases:
 *   ' ' / '\t' / '\n' / '\r' / '\f' / '\v' → 1
 *   '0' / 'A' / any other → 0
 */
int
gj_wchar_isspace_u_8414(uint32_t u32Ch)
{
	(void)NULL;
	return b8414_isspace(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wchar_isspace_u_8414(uint32_t u32Ch)
    __attribute__((alias("gj_wchar_isspace_u_8414")));
