/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6241: BT.709 8-bit RGB→Y luma stub.
 *
 * Surface (unique symbols):
 *   uint8_t gj_cs_bt709_luma8_6241(uint8_t r, uint8_t g, uint8_t b);
 *     - Y = (54*R + 183*G + 19*B) >> 8  (approx 0.2126/0.7152/0.0722).
 *       Result in 0..255. Pure integer; no float.
 *   uint8_t __gj_cs_bt709_luma8_6241  (alias)
 *   __libcgj_batch6241_marker = "libcgj-batch6241"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs — bt709_luma8, bt2020_luma8, bt709_cb_u8, bt2020_cb_u8,
 * bt709_cr_u8, bt2020_cr_u8, y_limited8_ok, c_limited8_ok,
 * matrix_id_ok, continuum + batch_id_6250). Unique
 * gj_cs_bt709_luma8_6241 surface only; no multi-def. Distinct from
 * gj_rgb_to_luma_bt709 (batch1686). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6241_marker[] = "libcgj-batch6241";

/* BT.709 fixed-point weights for /256 scale (sum 256). */
#define B6241_WR 54u
#define B6241_WG 183u
#define B6241_WB 19u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6241_ch8(uint8_t u8V)
{
	return (uint32_t)u8V & 0xffu;
}

static uint8_t
b6241_luma(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	uint32_t u32Y;

	u32Y = B6241_WR * b6241_ch8(u8R) +
	    B6241_WG * b6241_ch8(u8G) +
	    B6241_WB * b6241_ch8(u8B);
	return (uint8_t)(u32Y >> 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_bt709_luma8_6241 - integer BT.709 luma (HDTV / sRGB-ish 8-bit).
 *
 * 8.0 fixed point (>> 8). Max weighted sum is 54*255+183*255+19*255 =
 * 65280; fits in uint32_t. Distinct from gj_rgb_to_luma_bt709 so both
 * may coexist at link time. Does not call libc. No parent wires.
 */
uint8_t
gj_cs_bt709_luma8_6241(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	(void)NULL;
	return b6241_luma(u8R, u8G, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_cs_bt709_luma8_6241(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_cs_bt709_luma8_6241")));
