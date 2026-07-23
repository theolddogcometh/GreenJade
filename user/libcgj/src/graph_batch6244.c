/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6244: BT.2020 8-bit RGB→Cb stub.
 *
 * Surface (unique symbols):
 *   uint8_t gj_cs_bt2020_cb_u8_6244(uint8_t r, uint8_t g, uint8_t b);
 *     - Cb ≈ 128 + ((-34*R - 94*G + 128*B) >> 8)  (BT.2020 NCL chroma).
 *       Result clamped to 0..255. Pure integer; no float.
 *   uint8_t __gj_cs_bt2020_cb_u8_6244  (alias)
 *   __libcgj_batch6244_marker = "libcgj-batch6244"
 *
 * Exclusive continuum CREATE-ONLY (6241-6250: color space BT.709 /
 * BT.2020 stubs). Unique gj_cs_bt2020_cb_u8_6244 surface only; no
 * multi-def. Distinct from gj_cs_bt709_cb_u8_6243. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6244_marker[] = "libcgj-batch6244";

/* BT.2020 Cb fixed-point weights for /256 (≈ -0.1396/-0.3604/+0.5). */
#define B6244_WR  (-34)
#define B6244_WG  (-94)
#define B6244_WB  128
#define B6244_BIAS 128

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b6244_clamp_u8(int32_t i32V)
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
b6244_cb(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	int32_t i32S;

	i32S = B6244_WR * (int32_t)u8R +
	    B6244_WG * (int32_t)u8G +
	    B6244_WB * (int32_t)u8B;
	i32S = B6244_BIAS + (i32S >> 8);
	return b6244_clamp_u8(i32S);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_bt2020_cb_u8_6244 - integer BT.2020 NCL Cb.
 *
 * Offset-binary 8-bit with mid-point 128. Paired with bt2020_luma8 /
 * bt2020_cr_u8 for UHD matrix stubs. Does not call libc. No parent wires.
 */
uint8_t
gj_cs_bt2020_cb_u8_6244(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	(void)NULL;
	return b6244_cb(u8R, u8G, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_cs_bt2020_cb_u8_6244(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_cs_bt2020_cb_u8_6244")));
