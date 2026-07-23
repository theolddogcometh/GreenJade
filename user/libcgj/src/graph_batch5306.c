/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5306: UHD 3840x2160 display-mode predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_is_uhd_u(uint32_t w, uint32_t h);
 *     - Return 1 when (w,h) is exactly 3840x2160 (UHD/4K), else 0.
 *   uint32_t __gj_mode_is_uhd_u  (alias)
 *   __libcgj_batch5306_marker = "libcgj-batch5306"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Unique gj_mode_is_uhd_u surface
 * only; no multi-def. Distinct from HD/FHD/QHD predicates.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5306_marker[] = "libcgj-batch5306";

/* UHD / 4K progressive frame size. */
#define B5306_UHD_W  3840u
#define B5306_UHD_H  2160u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5306_is_uhd(uint32_t u32W, uint32_t u32H)
{
	return (u32W == B5306_UHD_W && u32H == B5306_UHD_H) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_is_uhd_u - true when dimensions are exactly UHD 3840x2160.
 *
 * w: frame width in pixels
 * h: frame height in pixels
 *
 * Returns 1 if w==3840 and h==2160, else 0. Exact match only; no
 * orientation swap. Pure data; no mode objects. No parent wires.
 */
uint32_t
gj_mode_is_uhd_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b5306_is_uhd(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_is_uhd_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_mode_is_uhd_u")));
