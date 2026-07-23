/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3585: UTF-16 low-surrogate predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_utf16_is_low_surr_u(uint16_t u);
 *     - Returns 1 if u is a low/trail surrogate U+DC00..U+DFFF, else 0.
 *   int __gj_utf16_is_low_surr_u  (alias)
 *   __libcgj_batch3585_marker = "libcgj-batch3585"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590). Distinct from
 * gj_utf16_is_low_surrogate (batch1488) — unique shortened _u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3585_marker[] = "libcgj-batch3585";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3585_is_low(uint16_t u16)
{
	return (u16 >= 0xDC00u && u16 <= 0xDFFFu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_utf16_is_low_surr_u - true when u is U+DC00..U+DFFF.
 */
int
gj_utf16_is_low_surr_u(uint16_t u16)
{
	(void)NULL;
	return b3585_is_low(u16);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_utf16_is_low_surr_u(uint16_t u16)
    __attribute__((alias("gj_utf16_is_low_surr_u")));
