/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5167: framebuffer pixel count (w * h).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_pixel_count_u(uint32_t w, uint32_t h);
 *     - Return w * h — total samples in a rectangular plane. Zero w or
 *       h yields 0; overflow yields 0.
 *   uint32_t __gj_fb_pixel_count_u  (alias)
 *   __libcgj_batch5167_marker = "libcgj-batch5167"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170). Distinct from
 * gj_fb_size_u (batch5165) — unique gj_fb_pixel_count_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5167_marker[] = "libcgj-batch5167";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pixel count: w * h. 0 on zero inputs or overflow. */
static uint32_t
b5167_pixel_count(uint32_t u32W, uint32_t u32H)
{
	if (u32W == 0u || u32H == 0u) {
		return 0u;
	}
	if (u32W > (UINT32_MAX / u32H)) {
		return 0u;
	}
	return u32W * u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_pixel_count_u - total pixel samples in a w x h plane.
 *
 * w: pixel width
 * h: pixel height
 *
 * Returns w * h, or 0 when either dimension is zero or the product
 * would overflow uint32_t. Self-contained; no parent wires.
 */
uint32_t
gj_fb_pixel_count_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b5167_pixel_count(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_pixel_count_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_fb_pixel_count_u")));
