/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5303: HD 1280x720 display-mode predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_is_hd_u(uint32_t w, uint32_t h);
 *     - Return 1 when (w,h) is exactly 1280x720 (HD/720p), else 0.
 *   uint32_t __gj_mode_is_hd_u  (alias)
 *   __libcgj_batch5303_marker = "libcgj-batch5303"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Unique gj_mode_is_hd_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5303_marker[] = "libcgj-batch5303";

/* HD / 720p progressive frame size. */
#define B5303_HD_W  1280u
#define B5303_HD_H  720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5303_is_hd(uint32_t u32W, uint32_t u32H)
{
	return (u32W == B5303_HD_W && u32H == B5303_HD_H) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_hd_u - true when dimensions are exactly HD 1280x720.
 *
 * w: frame width in pixels
 * h: frame height in pixels
 *
 * Returns 1 if w==1280 and h==720, else 0. Exact match only; no
 * orientation swap. Pure data; no mode objects. No parent wires.
 */
uint32_t
gj_mode_is_hd_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b5303_is_hd(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_is_hd_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_mode_is_hd_u")));
