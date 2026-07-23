/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1681: pack 8-bit R,G,B into 0x00RRGGBB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgb_pack_u32(uint8_t r, uint8_t g, uint8_t b);
 *     — Returns 0x00RRGGBB (high byte zero). Channels are masked to 8 bits.
 *   uint32_t __gj_rgb_pack_u32  (alias)
 *   __libcgj_batch1681_marker = "libcgj-batch1681"
 *
 * Color / pixel exclusive integer wave. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1681_marker[] = "libcgj-batch1681";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1681_ch8(uint8_t u8V)
{
	return (uint32_t)u8V & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_pack_u32 — pack R,G,B into 0x00RRGGBB.
 *
 * Layout (MSB→LSB): 00 | R | G | B. Matches common XRGB8888 / 0x00RRGGBB
 * desktop pixel words (not BGR).
 */
uint32_t
gj_rgb_pack_u32(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	return (b1681_ch8(u8R) << 16) |
	    (b1681_ch8(u8G) << 8) |
	    b1681_ch8(u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgb_pack_u32(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_rgb_pack_u32")));
