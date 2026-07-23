/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4877: approximate BT.601 luma of a packed RGB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgb_luma_u(uint32_t c);
 *     - Y = (77*R + 150*G + 29*B) >> 8 from packed 0xAARRGGBB /
 *       0x00RRGGBB. Result in 0..255. Alpha is ignored.
 *   uint32_t __gj_rgb_luma_u  (alias)
 *   __libcgj_batch4877_marker = "libcgj-batch4877"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880). Distinct
 * from gj_rgb_to_luma_bt601 (batch1685) which takes separate channels —
 * unique gj_rgb_luma_u packed-word surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4877_marker[] = "libcgj-batch4877";

/* BT.601 fixed-point weights for /256 scale (sum 256). */
#define B4877_WR  77u
#define B4877_WG  150u
#define B4877_WB  29u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4877_luma(uint32_t u32C)
{
	uint32_t u32R;
	uint32_t u32G;
	uint32_t u32B;
	uint32_t u32Y;

	u32R = (u32C >> 16) & 0xffu;
	u32G = (u32C >> 8) & 0xffu;
	u32B = u32C & 0xffu;
	u32Y = B4877_WR * u32R + B4877_WG * u32G + B4877_WB * u32B;
	return u32Y >> 8;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_luma_u - approximate BT.601 luma of a packed RGB/RGBA word.
 *
 * c: packed color (0xAARRGGBB or 0x00RRGGBB)
 *
 * Y = (77*R + 150*G + 29*B) >> 8. Classic PC / SDTV weights via 8.0
 * fixed point. Max weighted sum is 77*255+150*255+29*255 = 65280, so
 * no overflow risk in a 32-bit accumulator. Alpha is ignored.
 * Self-contained; no parent wires.
 */
uint32_t
gj_rgb_luma_u(uint32_t u32C)
{
	(void)NULL;
	return b4877_luma(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgb_luma_u(uint32_t u32C)
    __attribute__((alias("gj_rgb_luma_u")));
