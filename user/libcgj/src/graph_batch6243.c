/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6243: BT.709 8-bit RGB→Cb stub.
 *
 * Surface (unique symbols):
 *   uint8_t gj_cs_bt709_cb_u8_6243(uint8_t r, uint8_t g, uint8_t b);
 *     - Cb ≈ 128 + ((-29*R - 99*G + 128*B) >> 8)  (BT.709 chroma blue).
 *       Result clamped to 0..255. Pure integer; no float.
 *   uint8_t __gj_cs_bt709_cb_u8_6243  (alias)
 *   __libcgj_batch6243_marker = "libcgj-batch6243"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_bt709_cb_u8_6243 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6243_marker[] = "libcgj-batch6243";

/* BT.709 Cb fixed-point weights for /256 scale (≈ -0.1146/-0.3854/+0.5). */
#define B6243_WR  (-29)
#define B6243_WG  (-99)
#define B6243_WB  128
#define B6243_BIAS 128

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b6243_clamp_u8(int32_t i32V)
{
	if (i32V < 0) {
		return 0u;
	}
	if (i32V > 255) {
		return 255u;
	}
	return (uint8_t)i32V;
}

static uint8_t
b6243_cb(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	int32_t i32S;

	i32S = B6243_WR * (int32_t)u8R +
	    B6243_WG * (int32_t)u8G +
	    B6243_WB * (int32_t)u8B;
	i32S = B6243_BIAS + (i32S >> 8);
	return b6243_clamp_u8(i32S);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_bt709_cb_u8_6243 - integer BT.709 Cb (chroma-blue difference).
 *
 * Offset-binary 8-bit with mid-point 128. Used by color-space policy
 * stubs before limited-range clamps. Does not call libc. No parent wires.
 */
uint8_t
gj_cs_bt709_cb_u8_6243(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	(void)NULL;
	return b6243_cb(u8R, u8G, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_cs_bt709_cb_u8_6243(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_cs_bt709_cb_u8_6243")));
