/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3583: UTF-16 surrogate unit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf16_is_surrogate_u(uint16_t u);
 *     - Returns 1 if u is in U+D800..U+DFFF (any surrogate code unit),
 *       else 0. Surrogate units are not Unicode scalar values.
 *   int __gj_utf16_is_surrogate_u  (alias)
 *   __libcgj_batch3583_marker = "libcgj-batch3583"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590). Distinct from
 * gj_utf16_is_surrogate (batch1486) — unique _u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3583_marker[] = "libcgj-batch3583";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3583_is_surrogate(uint16_t u16)
{
	return (u16 >= 0xD800u && u16 <= 0xDFFFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_is_surrogate_u - true when u is any UTF-16 surrogate unit.
 */
int
gj_utf16_is_surrogate_u(uint16_t u16)
{
	(void)NULL;
	return b3583_is_surrogate(u16);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf16_is_surrogate_u(uint16_t u16)
    __attribute__((alias("gj_utf16_is_surrogate_u")));
