/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8417: wchar-class isprint predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_wchar_isprint_u_8417(uint32_t ch);
 *     - Return 1 if ch is a printable ASCII code unit 0x20..0x7e; else 0.
 *   int __gj_wchar_isprint_u_8417  (alias)
 *   __libcgj_batch8417_marker = "libcgj-batch8417"
 *
 * Wchar class stubs wave (8411-8420). Distinct from iswprint / isprint
 * and gj_ctype_isprint_u_8219 — unique _u_8417 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8417_marker[] = "libcgj-batch8417";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is in 0x20..0x7e (printable ASCII). */
static int
b8417_isprint(uint32_t u32Ch)
{
	if (u32Ch >= 0x20u && u32Ch <= 0x7eu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wchar_isprint_u_8417 - 1 iff ch is printable ASCII (SP..~).
 *
 * Edge cases:
 *   0x20..0x7e → 1
 *   0x00..0x1f / 0x7f / 0x80.. → 0
 */
int
gj_wchar_isprint_u_8417(uint32_t u32Ch)
{
	(void)NULL;
	return b8417_isprint(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wchar_isprint_u_8417(uint32_t u32Ch)
    __attribute__((alias("gj_wchar_isprint_u_8417")));
