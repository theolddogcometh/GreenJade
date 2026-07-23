/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6242: BT.2020 8-bit RGB→Y luma stub.
 *
 * Surface (unique symbols):
 *   uint8_t gj_cs_bt2020_luma8_6242(uint8_t r, uint8_t g, uint8_t b);
 *     - Y = (67*R + 174*G + 15*B) >> 8  (approx 0.2627/0.6780/0.0593).
 *       Result in 0..255. Pure integer; no float.
 *   uint8_t __gj_cs_bt2020_luma8_6242  (alias)
 *   __libcgj_batch6242_marker = "libcgj-batch6242"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_bt2020_luma8_6242 surface only; no
 * multi-def. Distinct from gj_cs_bt709_luma8_6241 / gj_rgb_to_luma_bt709.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6242_marker[] = "libcgj-batch6242";

/* BT.2020 fixed-point weights for /256 scale (sum 256). */
#define B6242_WR 67u
#define B6242_WG 174u
#define B6242_WB 15u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6242_ch8(uint8_t u8V)
{
	return (uint32_t)u8V & 0xffu;
}

static uint8_t
b6242_luma(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	uint32_t u32Y;

	u32Y = B6242_WR * b6242_ch8(u8R) +
	    B6242_WG * b6242_ch8(u8G) +
	    B6242_WB * b6242_ch8(u8B);
	return (uint8_t)(u32Y >> 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_bt2020_luma8_6242 - integer BT.2020 non-constant-luminance Y.
 *
 * 8.0 fixed point (>> 8). Max weighted sum is 67*255+174*255+15*255 =
 * 65280; fits in uint32_t. Pure data-path stub for UHD color-space
 * policy math. Does not call libc. No parent wires.
 */
uint8_t
gj_cs_bt2020_luma8_6242(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	(void)NULL;
	return b6242_luma(u8R, u8G, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_cs_bt2020_luma8_6242(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_cs_bt2020_luma8_6242")));
