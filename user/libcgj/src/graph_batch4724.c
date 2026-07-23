/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4724: minimum angular distance mod 360°.
 *
 * Surface (unique symbols):
 *   uint32_t gj_angle_diff_u(uint32_t a, uint32_t b);
 *     - Smallest unsigned degree distance between a and b on the circle
 *       (result in [0, 180]). Both inputs reduced mod 360 first.
 *   uint32_t __gj_angle_diff_u  (alias)
 *   __libcgj_batch4724_marker = "libcgj-batch4724"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730). Unique gj_angle_diff_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4724_marker[] = "libcgj-batch4724";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Min circular distance on a 360° wheel.
 * d = |a-b| mod 360; if d > 180 then 360 - d.
 */
static uint32_t
b4724_angle_diff(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32Aa;
	uint32_t u32Bb;
	uint32_t u32D;

	u32Aa = u32A % 360u;
	u32Bb = u32B % 360u;
	if (u32Aa >= u32Bb) {
		u32D = u32Aa - u32Bb;
	} else {
		u32D = u32Bb - u32Aa;
	}
	if (u32D > 180u) {
		u32D = 360u - u32D;
	}
	return u32D;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_angle_diff_u - minimum degree distance between two angles mod 360.
 *
 * a, b: angles in whole degrees (any uint32_t; reduced mod 360)
 *
 * Returns the shortest arc in [0, 180]. Self-contained; no parent wires.
 */
uint32_t
gj_angle_diff_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4724_angle_diff(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_angle_diff_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_angle_diff_u")));
