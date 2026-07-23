/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1683: pack 8-bit R,G,B,A into 0xAARRGGBB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgba_pack_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
 *     — Returns 0xAARRGGBB. Channels masked to 8 bits.
 *   uint32_t __gj_rgba_pack_u32  (alias)
 *   __libcgj_batch1683_marker = "libcgj-batch1683"
 *
 * Color / pixel exclusive integer wave. Distinct from gj_rgb_pack_u32
 * (batch1681, no alpha). Does NOT redefine prior batch symbols.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1683_marker[] = "libcgj-batch1683";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1683_ch8(uint8_t u8V)
{
	return (uint32_t)u8V & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgba_pack_u32 — pack R,G,B,A into 0xAARRGGBB.
 *
 * Layout (MSB→LSB): A | R | G | B. Matches common ARGB8888 desktop words.
 */
uint32_t
gj_rgba_pack_u32(uint8_t u8R, uint8_t u8G, uint8_t u8B, uint8_t u8A)
{
	return (b1683_ch8(u8A) << 24) |
	    (b1683_ch8(u8R) << 16) |
	    (b1683_ch8(u8G) << 8) |
	    b1683_ch8(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgba_pack_u32(uint8_t u8R, uint8_t u8G, uint8_t u8B, uint8_t u8A)
    __attribute__((alias("gj_rgba_pack_u32")));
