/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5307: total pixel count for a display mode.
 *
 * Surface (unique symbols):
 *   uint64_t gj_mode_pixels_u(uint32_t w, uint32_t h);
 *     - Return (uint64_t)w * (uint64_t)h, the mode's total pixel count.
 *   uint64_t __gj_mode_pixels_u  (alias)
 *   __libcgj_batch5307_marker = "libcgj-batch5307"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Unique gj_mode_pixels_u surface
 * only; no multi-def. Widens to uint64_t so large modes do not wrap.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5307_marker[] = "libcgj-batch5307";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5307_pixels(uint32_t u32W, uint32_t u32H)
{
	return (uint64_t)u32W * (uint64_t)u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_pixels_u - total pixel count for a rectangular mode.
 *
 * w: frame width in pixels
 * h: frame height in pixels
 *
 * Returns w*h as uint64_t (full product, no 32-bit wrap). Pure multiply;
 * does not allocate a framebuffer. No parent wires.
 */
uint64_t
gj_mode_pixels_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b5307_pixels(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mode_pixels_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_mode_pixels_u")));
