/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8419: wchar-class nul predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_wchar_nul_u_8419(uint32_t ch);
 *     - Return 1 if ch is the wide-character null (code unit 0); else 0.
 *   int __gj_wchar_nul_u_8419  (alias)
 *   __libcgj_batch8419_marker = "libcgj-batch8419"
 *
 * Wchar class stubs wave (8411-8420). Distinct from iswcntrl / L'\0'
 * tests elsewhere — unique _u_8419 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8419_marker[] = "libcgj-batch8419";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ch is the null wide-character code unit. */
static int
b8419_nul(uint32_t u32Ch)
{
	if (u32Ch == 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wchar_nul_u_8419 - 1 iff ch is L'\0' (code unit 0).
 *
 * Edge cases:
 *   0 → 1
 *   1..UINT32_MAX → 0
 */
int
gj_wchar_nul_u_8419(uint32_t u32Ch)
{
	(void)NULL;
	return b8419_nul(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wchar_nul_u_8419(uint32_t u32Ch)
    __attribute__((alias("gj_wchar_nul_u_8419")));
