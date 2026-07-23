/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1688: premultiply RGBA8 color by alpha.
 *
 * Surface (unique symbols):
 *   int gj_premultiply_rgba_u8(uint8_t *rgba);
 *     — In-place: R,G,B := R*A/255, G*A/255, B*A/255 (rounded).
 *       A unchanged. NULL → -1; else 0. Pure integer.
 *   int __gj_premultiply_rgba_u8  (alias)
 *   __libcgj_batch1688_marker = "libcgj-batch1688"
 *
 * Color / pixel exclusive integer wave. Pairs with unpremultiply
 * (batch1689). Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1688_marker[] = "libcgj-batch1688";

/* ---- freestanding helpers ---------------------------------------------- */

/* (c * a + 127) / 255 — rounded scale into premultiplied space. */
static uint8_t
b1688_mul_a(uint8_t u8C, uint8_t u8A)
{
	return (uint8_t)(((uint32_t)u8C * (uint32_t)u8A + 127u) / 255u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_premultiply_rgba_u8 — convert straight RGBA8 to premultiplied.
 *
 * pRgba: 4 bytes {R,G,B,A}. Alpha is left as-is; color channels are
 * scaled by A/255 with nearest rounding.
 */
int
gj_premultiply_rgba_u8(uint8_t *pRgba)
{
	uint8_t u8A;

	if (pRgba == NULL) {
		return -1;
	}

	u8A = pRgba[3];
	pRgba[0] = b1688_mul_a(pRgba[0], u8A);
	pRgba[1] = b1688_mul_a(pRgba[1], u8A);
	pRgba[2] = b1688_mul_a(pRgba[2], u8A);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_premultiply_rgba_u8(uint8_t *pRgba)
    __attribute__((alias("gj_premultiply_rgba_u8")));
