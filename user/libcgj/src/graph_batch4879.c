/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4879: test whether a packed RGB word is black.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgb_is_black_u(uint32_t c);
 *     - Return 1 when R==G==B==0 ((c & 0x00ffffff) == 0); else 0.
 *       Alpha is ignored so both 0x00000000 and 0xFF000000 count as black.
 *   uint32_t __gj_rgb_is_black_u  (alias)
 *   __libcgj_batch4879_marker = "libcgj-batch4879"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880). Distinct
 * from gj_rb_is_black_p (batch2342, red-black tree colors) — unique
 * gj_rgb_is_black_u packed-pixel surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4879_marker[] = "libcgj-batch4879";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4879_is_black(uint32_t u32C)
{
	if ((u32C & 0x00ffffffu) == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_is_black_u - true (1) when packed RGB channels are all zero.
 *
 * c: packed color (0xAARRGGBB or 0x00RRGGBB)
 *
 * Alpha is ignored. Returns 1 for black, 0 otherwise.
 * Self-contained; no parent wires.
 */
uint32_t
gj_rgb_is_black_u(uint32_t u32C)
{
	(void)NULL;
	return b4879_is_black(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgb_is_black_u(uint32_t u32C)
    __attribute__((alias("gj_rgb_is_black_u")));
