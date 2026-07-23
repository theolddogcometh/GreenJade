/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5165: framebuffer total plane size in bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_size_u(uint32_t w, uint32_t h, uint32_t bpp);
 *     - Return w * h * (bpp / 8) — total bytes for a tight framebuffer
 *       of width w, height h, depth bpp. Zero inputs yield 0; overflow
 *       yields 0.
 *   uint32_t __gj_fb_size_u  (alias)
 *   __libcgj_batch5165_marker = "libcgj-batch5165"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170). Distinct from
 * gj_fb_stride_u (batch5164) — unique gj_fb_size_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5165_marker[] = "libcgj-batch5165";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked u32 multiply; 0 on zero or overflow. */
static uint32_t
b5165_mul_u32(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	if (u32A > (UINT32_MAX / u32B)) {
		return 0u;
	}
	return u32A * u32B;
}

/* Plane size: w * h * (bpp / 8). */
static uint32_t
b5165_size(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
{
	uint32_t u32BppBytes;
	uint32_t u32Pixels;
	uint32_t u32Bytes;

	if (u32W == 0u || u32H == 0u || u32Bpp == 0u) {
		return 0u;
	}
	u32BppBytes = u32Bpp / 8u;
	if (u32BppBytes == 0u) {
		return 0u;
	}
	u32Pixels = b5165_mul_u32(u32W, u32H);
	if (u32Pixels == 0u) {
		return 0u;
	}
	u32Bytes = b5165_mul_u32(u32Pixels, u32BppBytes);
	return u32Bytes;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_size_u - total bytes for a tight w x h framebuffer at bpp.
 *
 * w:   pixel width
 * h:   pixel height
 * bpp: bits per pixel
 *
 * Returns w * h * (bpp / 8), or 0 on zero inputs, bpp < 8, or overflow.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fb_size_u(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
{
	(void)NULL;
	return b5165_size(u32W, u32H, u32Bpp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_size_u(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
    __attribute__((alias("gj_fb_size_u")));
