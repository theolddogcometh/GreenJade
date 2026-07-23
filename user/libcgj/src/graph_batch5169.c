/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5169: pack 8-bit RGB channels into RGB565.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_rgb565_u(uint32_t r, uint32_t g, uint32_t b);
 *     - Pack 8-bit channel values into a 16-bit RGB565 word:
 *         ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
 *       Only the low 8 bits of each channel are used.
 *   uint32_t __gj_fb_rgb565_u  (alias)
 *   __libcgj_batch5169_marker = "libcgj-batch5169"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170). Pairs with
 * gj_fb_bpp16_u (batch5163) — unique gj_fb_rgb565_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5169_marker[] = "libcgj-batch5169";

/* ---- freestanding helpers ---------------------------------------------- */

/* 8-bit RGB → 16-bit RGB565 word in the low 16 bits. */
static uint32_t
b5169_rgb565(uint32_t u32R, uint32_t u32G, uint32_t u32B)
{
	uint32_t u32R5;
	uint32_t u32G6;
	uint32_t u32B5;

	u32R5 = (u32R & 0xffu) >> 3;
	u32G6 = (u32G & 0xffu) >> 2;
	u32B5 = (u32B & 0xffu) >> 3;
	return (u32R5 << 11) | (u32G6 << 5) | u32B5;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_rgb565_u - pack 8-bit r,g,b into a 16-bit RGB565 pixel word.
 *
 * r: red channel (0..255; only low 8 bits used)
 * g: green channel (0..255; only low 8 bits used)
 * b: blue channel (0..255; only low 8 bits used)
 *
 * Returns a value in [0, 0xFFFF] with R in bits 15..11, G in 10..5,
 * B in 4..0. Self-contained; no parent wires.
 */
uint32_t
gj_fb_rgb565_u(uint32_t u32R, uint32_t u32G, uint32_t u32B)
{
	(void)NULL;
	return b5169_rgb565(u32R, u32G, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_rgb565_u(uint32_t u32R, uint32_t u32G, uint32_t u32B)
    __attribute__((alias("gj_fb_rgb565_u")));
