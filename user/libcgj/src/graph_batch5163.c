/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5163: framebuffer 16 bits-per-pixel constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_bpp16_u(void);
 *     - Returns the canonical 16-bpp depth for RGB565/ARGB1555
 *       framebuffer planes (2 bytes per pixel).
 *   uint32_t __gj_fb_bpp16_u  (alias)
 *   __libcgj_batch5163_marker = "libcgj-batch5163"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170: fb_bpp32_u, fb_bpp24_u,
 * fb_bpp16_u, fb_stride_u, fb_size_u, fb_offset_u, fb_pixel_count_u,
 * fb_is_valid_u, fb_rgb565_u, batch_id_5170). Distinct from
 * gj_fb_bpp32_u / gj_fb_bpp24_u — unique gj_fb_bpp16_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5163_marker[] = "libcgj-batch5163";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5163_bpp16(void)
{
	return 16u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_bpp16_u - report the 16 bits-per-pixel framebuffer depth.
 *
 * Always returns 16. Pairs with gj_fb_rgb565_u (batch5169) for 16-bpp
 * plane math. Self-contained; no parent wires.
 */
uint32_t
gj_fb_bpp16_u(void)
{
	(void)NULL;
	return b5163_bpp16();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_bpp16_u(void)
    __attribute__((alias("gj_fb_bpp16_u")));
