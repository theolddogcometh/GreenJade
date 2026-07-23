/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5162: framebuffer 24 bits-per-pixel constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fb_bpp24_u(void);
 *     - Returns the canonical 24-bpp depth for packed RGB framebuffer
 *       planes (3 bytes per pixel).
 *   uint32_t __gj_fb_bpp24_u  (alias)
 *   __libcgj_batch5162_marker = "libcgj-batch5162"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170: fb_bpp32_u, fb_bpp24_u,
 * fb_bpp16_u, fb_stride_u, fb_size_u, fb_offset_u, fb_pixel_count_u,
 * fb_is_valid_u, fb_rgb565_u, batch_id_5170). Distinct from
 * gj_fb_bpp32_u (batch5161) — unique gj_fb_bpp24_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5162_marker[] = "libcgj-batch5162";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5162_bpp24(void)
{
	return 24u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fb_bpp24_u - report the 24 bits-per-pixel framebuffer depth.
 *
 * Always returns 24. Used for packed RGB24 plane layout hints.
 * Self-contained; no parent wires.
 */
uint32_t
gj_fb_bpp24_u(void)
{
	(void)NULL;
	return b5162_bpp24();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fb_bpp24_u(void)
    __attribute__((alias("gj_fb_bpp24_u")));
