/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5168: framebuffer geometry validity predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_is_valid_u(uint32_t w, uint32_t h, uint32_t bpp);
 *     - Return 1 when w > 0, h > 0, and bpp is a supported continuum
 *       depth (16, 24, or 32); else 0.
 *   uint32_t __gj_fb_is_valid_u  (alias)
 *   __libcgj_batch5168_marker = "libcgj-batch5168"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170). Distinct from
 * gj_fb_size_u / bpp constants — unique gj_fb_is_valid_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5168_marker[] = "libcgj-batch5168";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if w,h positive and bpp in {16,24,32}. */
static uint32_t
b5168_is_valid(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
{
	if (u32W == 0u || u32H == 0u) {
		return 0u;
	}
	if (u32Bpp == 16u || u32Bpp == 24u || u32Bpp == 32u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_is_valid_u - predicate for a usable framebuffer geometry.
 *
 * w:   pixel width (must be non-zero)
 * h:   pixel height (must be non-zero)
 * bpp: bits per pixel; accepted depths are 16, 24, and 32 (matches
 *      gj_fb_bpp16_u / gj_fb_bpp24_u / gj_fb_bpp32_u in this wave)
 *
 * Returns 1 when geometry is valid, else 0. Self-contained; no parent
 * wires.
 */
uint32_t
gj_fb_is_valid_u(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
{
	(void)NULL;
	return b5168_is_valid(u32W, u32H, u32Bpp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_is_valid_u(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
    __attribute__((alias("gj_fb_is_valid_u")));
