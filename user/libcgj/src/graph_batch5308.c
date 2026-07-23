/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5308: simplified aspect-ratio numerator.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_aspect_num_u(uint32_t w, uint32_t h);
 *     - Return w / gcd(w,h), the reduced aspect-ratio numerator.
 *       When gcd is 0 (both zero), returns 0.
 *   uint32_t __gj_mode_aspect_num_u  (alias)
 *   __libcgj_batch5308_marker = "libcgj-batch5308"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Unique gj_mode_aspect_num_u
 * surface only; no multi-def. Local Euclidean GCD (no gj_gcd_* calls).
 * Pair with gj_mode_aspect_den_u (batch5309). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5308_marker[] = "libcgj-batch5308";

/* ---- freestanding helpers ---------------------------------------------- */

/* Local Euclidean GCD; gcd(0,0) returns 0. */
static uint32_t
b5308_gcd(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32T;

	while (u32B != 0u) {
		u32T = u32B;
		u32B = u32A % u32B;
		u32A = u32T;
	}
	return u32A;
}

static uint32_t
b5308_aspect_num(uint32_t u32W, uint32_t u32H)
{
	uint32_t u32G;

	u32G = b5308_gcd(u32W, u32H);
	if (u32G == 0u) {
		return 0u;
	}
	return u32W / u32G;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_aspect_num_u - reduced aspect-ratio numerator (w / gcd).
 *
 * w: frame width in pixels
 * h: frame height in pixels
 *
 * Returns w/gcd(w,h). Example: 1920x1080 -> 16. When both are 0,
 * returns 0. Local GCD only; does not redefine gj_gcd_*. No parent wires.
 */
uint32_t
gj_mode_aspect_num_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b5308_aspect_num(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_aspect_num_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_mode_aspect_num_u")));
