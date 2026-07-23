/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6468: screenshot raw byte-size estimate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_byte_est_6468(uint32_t width, uint32_t height,
 *                                   uint32_t bpp);
 *     - Soft raw byte estimate: (width * height * bpp) / 8 with
 *       saturation at 0xffffffff. Zero dims or bpp → 0. No overflow
 *       wrap (checked multiply). Pure integer.
 *   uint32_t __gj_sshot_byte_est_6468  (alias)
 *   __libcgj_batch6468_marker = "libcgj-batch6468"
 *
 * Exclusive continuum CREATE-ONLY (6461-6470: screenshot path/size stubs —
 * path_len_ok_6461, path_cap_ok_6462, path_depth_ok_6463, ext_ok_6464,
 * width_ok_6465, height_ok_6466, bpp_ok_6467, byte_est_6468, size_ok_6469,
 * path_size_ready_6470). Unique surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6468_marker[] = "libcgj-batch6468";

#define B6468_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6468_mul_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	if (u32A > (B6468_U32_MAX / u32B)) {
		return B6468_U32_MAX;
	}
	return u32A * u32B;
}

static uint32_t
b6468_byte_est(uint32_t u32W, uint32_t u32H, uint32_t u32Bpp)
{
	uint32_t u32Px;
	uint32_t u32Bits;

	if (u32W == 0u || u32H == 0u || u32Bpp == 0u) {
		return 0u;
	}
	u32Px = b6468_mul_sat(u32W, u32H);
	if (u32Px == B6468_U32_MAX) {
		return B6468_U32_MAX;
	}
	u32Bits = b6468_mul_sat(u32Px, u32Bpp);
	if (u32Bits == B6468_U32_MAX) {
		return B6468_U32_MAX;
	}
	return u32Bits / 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_byte_est_6468 - soft screenshot raw frame byte estimate.
 *
 * width:  soft width in pixels
 * height: soft height in pixels
 * bpp:    soft bits per pixel
 *
 * Returns (w*h*bpp)/8 saturating at UINT32_MAX, or 0 if any arg is 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_sshot_byte_est_6468(uint32_t width, uint32_t height, uint32_t bpp)
{
	(void)NULL;
	return b6468_byte_est(width, height, bpp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_byte_est_6468(uint32_t width, uint32_t height,
    uint32_t bpp)
    __attribute__((alias("gj_sshot_byte_est_6468")));
