/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8289: UTF-8 overlong lead guard probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_utf8_overlong_guard_u_8289(uint32_t b);
 *     - Soft single-byte overlong guard on a candidate lead: return 0
 *       if low 8 bits are classic 2-byte overlong leads C0/C1; return 1
 *       otherwise. Does not inspect second-byte overlong windows.
 *   uint32_t __gj_utf8_overlong_guard_u_8289  (alias)
 *   __libcgj_batch8289_marker = "libcgj-batch8289"
 *
 * Exclusive continuum CREATE-ONLY (8281-8290: UTF-8 / byte class probe
 * stubs — is_ascii, lead1, lead2, lead3, lead4, cont, seqlen,
 * valid_lead, overlong_guard, batch_id_8290). No full decoder.
 * Unique gj_utf8_overlong_guard_u_8289 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8289_marker[] = "libcgj-batch8289";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8289_overlong_guard(uint32_t u32B)
{
	uint32_t u8;

	u8 = u32B & 0xFFu;
	/* C0/C1 can only encode overlong 2-byte forms of ASCII. */
	if (u8 == 0xC0u || u8 == 0xC1u)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf8_overlong_guard_u_8289 - 0 for C0/C1 overlong leads, else 1.
 *
 * b: candidate lead octet (only low 8 bits matter)
 *
 * Returns 0 when b is C0 or C1 (classic 2-byte overlong leads), else 1.
 * Soft single-byte guard only; E0/F0 second-byte overlong windows are
 * out of scope (no full decoder). No parent wires.
 */
uint32_t
gj_utf8_overlong_guard_u_8289(uint32_t u32B)
{
	(void)NULL;
	return b8289_overlong_guard(u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_utf8_overlong_guard_u_8289(uint32_t u32B)
    __attribute__((alias("gj_utf8_overlong_guard_u_8289")));
