/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4872: pack R,G,B,A channels into 0xAARRGGBB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgba_pack_u(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
 *     - Pack unsigned channel words (masked to 8 bits) into a single
 *       0xAARRGGBB pixel word.
 *   uint32_t __gj_rgba_pack_u  (alias)
 *   __libcgj_batch4872_marker = "libcgj-batch4872"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880). Distinct
 * from gj_rgba_pack_u32 (batch1683) and gj_rgb_pack_u (batch4871) —
 * unique gj_rgba_pack_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4872_marker[] = "libcgj-batch4872";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4872_ch8(uint32_t u32V)
{
	return u32V & 0xffu;
}

static uint32_t
b4872_pack(uint32_t u32R, uint32_t u32G, uint32_t u32B, uint32_t u32A)
{
	return (b4872_ch8(u32A) << 24) |
	    (b4872_ch8(u32R) << 16) |
	    (b4872_ch8(u32G) << 8) |
	    b4872_ch8(u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgba_pack_u - pack R,G,B,A into 0xAARRGGBB.
 *
 * r, g, b, a: channel values (only the low 8 bits are used)
 *
 * Layout (MSB→LSB): A | R | G | B. Matches common ARGB8888 desktop
 * words. Self-contained; no parent wires.
 */
uint32_t
gj_rgba_pack_u(uint32_t u32R, uint32_t u32G, uint32_t u32B, uint32_t u32A)
{
	(void)NULL;
	return b4872_pack(u32R, u32G, u32B, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgba_pack_u(uint32_t u32R, uint32_t u32G, uint32_t u32B,
    uint32_t u32A)
    __attribute__((alias("gj_rgba_pack_u")));
