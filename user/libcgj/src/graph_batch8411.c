/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8411: wchar-class isascii predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_wchar_isascii_u_8411(uint32_t ch);
 *     - Return 1 if ch is a classic 7-bit ASCII code unit (0..127); else 0.
 *   int __gj_wchar_isascii_u_8411  (alias)
 *   __libcgj_batch8411_marker = "libcgj-batch8411"
 *
 * Wchar class stubs wave (8411-8420). Distinct from iswascii / isascii
 * and gj_ctype_isascii_u_8211 — unique _u_8411 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8411_marker[] = "libcgj-batch8411";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 0..127. */
static int
b8411_isascii(uint32_t u32Ch)
{
	if (u32Ch <= 127u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wchar_isascii_u_8411 - 1 iff ch is classic 7-bit ASCII.
 *
 * Edge cases:
 *   0..127 → 1
 *   128..UINT32_MAX → 0
 */
int
gj_wchar_isascii_u_8411(uint32_t u32Ch)
{
	(void)NULL;
	return b8411_isascii(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wchar_isascii_u_8411(uint32_t u32Ch)
    __attribute__((alias("gj_wchar_isascii_u_8411")));
