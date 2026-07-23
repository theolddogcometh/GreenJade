/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5164: framebuffer row stride in bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_stride_u(uint32_t w, uint32_t bpp);
 *     - Return w * (bpp / 8) — tight row pitch for width w at depth bpp.
 *       Integer division drops fractional bits. Zero w or bpp yields 0;
 *       overflow yields 0.
 *   uint32_t __gj_fb_stride_u  (alias)
 *   __libcgj_batch5164_marker = "libcgj-batch5164"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170). Distinct from
 * gj_buffer_stride_bytes (batch1852) / gj_row_pitch_align (batch1856) —
 * unique gj_fb_stride_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5164_marker[] = "libcgj-batch5164";

/* ---- freestanding helpers ---------------------------------------------- */

/* Tight stride: w * (bpp / 8). 0 on zero inputs or uint32 overflow. */
static uint32_t
b5164_stride(uint32_t u32W, uint32_t u32Bpp)
{
	uint32_t u32BppBytes;
	uint32_t u32Stride;

	if (u32W == 0u || u32Bpp == 0u) {
		return 0u;
	}
	u32BppBytes = u32Bpp / 8u;
	if (u32BppBytes == 0u) {
		return 0u;
	}
	if (u32W > (UINT32_MAX / u32BppBytes)) {
		return 0u;
	}
	u32Stride = u32W * u32BppBytes;
	return u32Stride;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_stride_u - bytes per framebuffer scanline (tight pitch).
 *
 * w:   pixel width of the plane
 * bpp: bits per pixel (e.g. 16, 24, 32)
 *
 * Returns w * (bpp / 8), or 0 when w or bpp is zero, bpp < 8, or the
 * product would overflow uint32_t. Self-contained; no parent wires.
 */
uint32_t
gj_fb_stride_u(uint32_t u32W, uint32_t u32Bpp)
{
	(void)NULL;
	return b5164_stride(u32W, u32Bpp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_stride_u(uint32_t u32W, uint32_t u32Bpp)
    __attribute__((alias("gj_fb_stride_u")));
