/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8412: wchar-class isdigit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_wchar_isdigit_u_8412(uint32_t ch);
 *     - Return 1 if ch is an ASCII decimal digit '0'..'9'; else 0.
 *   int __gj_wchar_isdigit_u_8412  (alias)
 *   __libcgj_batch8412_marker = "libcgj-batch8412"
 *
 * Wchar class stubs wave (8411-8420). Distinct from iswdigit / isdigit
 * and gj_ctype_isdigit_u_8212 — unique _u_8412 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8412_marker[] = "libcgj-batch8412";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in '0'..'9'. */
static int
b8412_isdigit(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wchar_isdigit_u_8412 - 1 iff ch is an ASCII decimal digit.
 *
 * Edge cases:
 *   '0'..'9' → 1
 *   'a' / 'A' / ' ' / any other → 0
 */
int
gj_wchar_isdigit_u_8412(uint32_t u32Ch)
{
	(void)NULL;
	return b8412_isdigit(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wchar_isdigit_u_8412(uint32_t u32Ch)
    __attribute__((alias("gj_wchar_isdigit_u_8412")));
