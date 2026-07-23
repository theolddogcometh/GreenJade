/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1686: BT.709 luma from 8-bit R,G,B (integer).
 *
 * Surface (unique symbols):
 *   uint8_t gj_rgb_to_luma_bt709(uint8_t r, uint8_t g, uint8_t b);
 *     — Y = (54*R + 183*G + 19*B) >> 8  (approx 0.2126/0.7152/0.0722).
 *       Result in 0..255. Pure integer; no float.
 *   uint8_t __gj_rgb_to_luma_bt709  (alias)
 *   __libcgj_batch1686_marker = "libcgj-batch1686"
 *
 * Color / pixel exclusive integer wave. Distinct from BT.601 (batch1685).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1686_marker[] = "libcgj-batch1686";

/* BT.709 fixed-point weights for /256 scale (sum 256). */
#define B1686_WR 54u
#define B1686_WG 183u
#define B1686_WB 19u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1686_ch8(uint8_t u8V)
{
	return (uint32_t)u8V & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_to_luma_bt709 — integer BT.709 luma (HDTV / sRGB-ish 8-bit).
 *
 * 8.0 fixed point (>> 8). Max weighted sum is 54*255+183*255+19*255 =
 * 65280; fits in uint32_t with room.
 */
uint8_t
gj_rgb_to_luma_bt709(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	uint32_t u32Y;

	u32Y = B1686_WR * b1686_ch8(u8R) +
	    B1686_WG * b1686_ch8(u8G) +
	    B1686_WB * b1686_ch8(u8B);
	return (uint8_t)(u32Y >> 8);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_rgb_to_luma_bt709(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_rgb_to_luma_bt709")));
