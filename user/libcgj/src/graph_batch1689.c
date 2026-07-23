/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1689: un-premultiply RGBA8 color by alpha.
 *
 * Surface (unique symbols):
 *   int gj_unpremultiply_rgba_u8(uint8_t *rgba);
 *     — In-place: if A==0 set R,G,B=0; else C := min(255, C*255/A).
 *       A unchanged. NULL → -1; else 0. Pure integer.
 *   int __gj_unpremultiply_rgba_u8  (alias)
 *   __libcgj_batch1689_marker = "libcgj-batch1689"
 *
 * Color / pixel exclusive integer wave. Inverse of premultiply
 * (batch1688). Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1689_marker[] = "libcgj-batch1689";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Recover straight color: (c * 255 + a/2) / a, clamped to 255.
 * Caller guarantees a != 0.
 */
static uint8_t
b1689_div_a(uint8_t u8C, uint8_t u8A)
{
	uint32_t u32V;

	u32V = ((uint32_t)u8C * 255u + ((uint32_t)u8A / 2u)) / (uint32_t)u8A;
	if (u32V > 255u) {
		return 255u;
	}
	return (uint8_t)u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpremultiply_rgba_u8 — convert premultiplied RGBA8 to straight.
 *
 * pRgba: 4 bytes {R,G,B,A}. Zero alpha forces RGB to 0 (no division by
 * zero). Non-zero alpha recovers approx. straight channels.
 */
int
gj_unpremultiply_rgba_u8(uint8_t *pRgba)
{
	uint8_t u8A;

	if (pRgba == NULL) {
		return -1;
	}

	u8A = pRgba[3];
	if (u8A == 0u) {
		pRgba[0] = 0u;
		pRgba[1] = 0u;
		pRgba[2] = 0u;
		return 0;
	}

	pRgba[0] = b1689_div_a(pRgba[0], u8A);
	pRgba[1] = b1689_div_a(pRgba[1], u8A);
	pRgba[2] = b1689_div_a(pRgba[2], u8A);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_unpremultiply_rgba_u8(uint8_t *pRgba)
    __attribute__((alias("gj_unpremultiply_rgba_u8")));
