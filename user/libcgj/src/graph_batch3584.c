/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3584: UTF-16 high-surrogate predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf16_is_high_surr_u(uint16_t u);
 *     - Returns 1 if u is a high/lead surrogate U+D800..U+DBFF, else 0.
 *   int __gj_utf16_is_high_surr_u  (alias)
 *   __libcgj_batch3584_marker = "libcgj-batch3584"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590). Distinct from
 * gj_utf16_is_high_surrogate (batch1487) — unique shortened _u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3584_marker[] = "libcgj-batch3584";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3584_is_high(uint16_t u16)
{
	return (u16 >= 0xD800u && u16 <= 0xDBFFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_is_high_surr_u - true when u is U+D800..U+DBFF.
 */
int
gj_utf16_is_high_surr_u(uint16_t u16)
{
	(void)NULL;
	return b3584_is_high(u16);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf16_is_high_surr_u(uint16_t u16)
    __attribute__((alias("gj_utf16_is_high_surr_u")));
