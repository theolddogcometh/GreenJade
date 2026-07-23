/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5166: framebuffer byte offset at (x, y).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_offset_u(uint32_t x, uint32_t y, uint32_t stride);
 *     - Return y * stride + x — linear byte (or unit) offset into a
 *       plane given row pitch stride. Overflow yields 0.
 *   uint32_t __gj_fb_offset_u  (alias)
 *   __libcgj_batch5166_marker = "libcgj-batch5166"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170). Distinct from
 * gj_fb_stride_u / gj_fb_size_u — unique gj_fb_offset_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5166_marker[] = "libcgj-batch5166";

/* ---- freestanding helpers ---------------------------------------------- */

/* Linear offset: y * stride + x. 0 on overflow. */
static uint32_t
b5166_offset(uint32_t u32X, uint32_t u32Y, uint32_t u32Stride)
{
	uint32_t u32Row;

	if (u32Y != 0u && u32Stride != 0u) {
		if (u32Y > (UINT32_MAX / u32Stride)) {
			return 0u;
		}
	}
	u32Row = u32Y * u32Stride;
	if (u32X > (UINT32_MAX - u32Row)) {
		return 0u;
	}
	return u32Row + u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_offset_u - byte (or unit) offset of sample (x, y) given stride.
 *
 * x:      horizontal position in the same units as stride elements
 *         (bytes if stride is row pitch in bytes; pixels if pixel-stride)
 * y:      scanline index
 * stride: row pitch (bytes or units per scanline)
 *
 * Returns y * stride + x, or 0 when the arithmetic would overflow
 * uint32_t. Self-contained; no parent wires.
 */
uint32_t
gj_fb_offset_u(uint32_t u32X, uint32_t u32Y, uint32_t u32Stride)
{
	(void)NULL;
	return b5166_offset(u32X, u32Y, u32Stride);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_offset_u(uint32_t u32X, uint32_t u32Y, uint32_t u32Stride)
    __attribute__((alias("gj_fb_offset_u")));
