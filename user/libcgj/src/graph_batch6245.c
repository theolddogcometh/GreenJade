/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6245: BT.709 8-bit RGB→Cr stub.
 *
 * Surface (unique symbols):
 *   uint8_t gj_cs_bt709_cr_u8_6245(uint8_t r, uint8_t g, uint8_t b);
 *     - Cr ≈ 128 + ((128*R - 116*G - 12*B) >> 8)  (BT.709 chroma red).
 *       Result clamped to 0..255. Pure integer; no float.
 *   uint8_t __gj_cs_bt709_cr_u8_6245  (alias)
 *   __libcgj_batch6245_marker = "libcgj-batch6245"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_bt709_cr_u8_6245 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6245_marker[] = "libcgj-batch6245";

/* BT.709 Cr fixed-point weights for /256 scale (≈ +0.5/-0.4542/-0.0458). */
#define B6245_WR  128
#define B6245_WG  (-116)
#define B6245_WB  (-12)
#define B6245_BIAS 128

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b6245_clamp_u8(int32_t i32V)
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
b6245_cr(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	int32_t i32S;

	i32S = B6245_WR * (int32_t)u8R +
	    B6245_WG * (int32_t)u8G +
	    B6245_WB * (int32_t)u8B;
	i32S = B6245_BIAS + (i32S >> 8);
	return b6245_clamp_u8(i32S);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_bt709_cr_u8_6245 - integer BT.709 Cr (chroma-red difference).
 *
 * Offset-binary 8-bit with mid-point 128. Complements bt709_luma8 and
 * bt709_cb_u8. Does not call libc. No parent wires.
 */
uint8_t
gj_cs_bt709_cr_u8_6245(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	(void)NULL;
	return b6245_cr(u8R, u8G, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_cs_bt709_cr_u8_6245(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_cs_bt709_cr_u8_6245")));
