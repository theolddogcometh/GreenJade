/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1685: BT.601 luma from 8-bit R,G,B (integer).
 *
 * Surface (unique symbols):
 *   uint8_t gj_rgb_to_luma_bt601(uint8_t r, uint8_t g, uint8_t b);
 *     — Y = (77*R + 150*G + 29*B) >> 8  (approx 0.299/0.587/0.114).
 *       Result in 0..255. Pure integer; no float.
 *   uint8_t __gj_rgb_to_luma_bt601  (alias)
 *   __libcgj_batch1685_marker = "libcgj-batch1685"
 *
 * Color / pixel exclusive integer wave. Distinct from BT.709 (batch1686).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1685_marker[] = "libcgj-batch1685";

/* BT.601 fixed-point weights for /256 scale (sum 256). */
#define B1685_WR 77u
#define B1685_WG 150u
#define B1685_WB 29u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1685_ch8(uint8_t u8V)
{
	return (uint32_t)u8V & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_to_luma_bt601 — integer BT.601 luma (rec.601-ish 8-bit).
 *
 * Classic PC / SDTV weights via 8.0 fixed point (>> 8). Max weighted
 * sum is 77*255+150*255+29*255 = 65280, so no 16-bit overflow risk in
 * a 32-bit accumulator.
 */
uint8_t
gj_rgb_to_luma_bt601(uint8_t u8R, uint8_t u8G, uint8_t u8B)
{
	uint32_t u32Y;

	u32Y = B1685_WR * b1685_ch8(u8R) +
	    B1685_WG * b1685_ch8(u8G) +
	    B1685_WB * b1685_ch8(u8B);
	return (uint8_t)(u32Y >> 8);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_rgb_to_luma_bt601(uint8_t u8R, uint8_t u8G, uint8_t u8B)
    __attribute__((alias("gj_rgb_to_luma_bt601")));
