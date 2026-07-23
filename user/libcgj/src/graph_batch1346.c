/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1346: simple 2D Hilbert-curve encode (16-bit).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hilbert2_encode_simple(uint16_t x, uint16_t y);
 *     — Map (x, y) on a 65536×65536 grid to a 32-bit Hilbert distance
 *       via the classic quadrant rotate/fold algorithm.
 *   uint32_t __gj_hilbert2_encode_simple  (alias)
 *   __libcgj_batch1346_marker = "libcgj-batch1346"
 *
 * Distinct from Morton encode (batch1344). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1346_marker[] = "libcgj-batch1346";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Rotate/flip a quadrant of side length u32S in-place (Hilbert transform).
 */
static void
b1346_rot(uint32_t u32S, uint32_t *pX, uint32_t *pY, uint32_t uRx, uint32_t uRy)
{
	uint32_t uT;

	if (uRy == 0u) {
		if (uRx == 1u) {
			*pX = u32S - 1u - *pX;
			*pY = u32S - 1u - *pY;
		}
		uT = *pX;
		*pX = *pY;
		*pY = uT;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hilbert2_encode_simple — 2D Hilbert index from (x, y).
 *
 * Operates on a 2^16 square (full uint16 range). Pure; all inputs accepted.
 */
uint32_t
gj_hilbert2_encode_simple(uint16_t u16X, uint16_t u16Y)
{
	uint32_t u32X;
	uint32_t u32Y;
	uint32_t u32D;
	uint32_t u32S;
	uint32_t uRx;
	uint32_t uRy;

	u32X = (uint32_t)u16X;
	u32Y = (uint32_t)u16Y;
	u32D = 0u;

	for (u32S = 32768u; u32S > 0u; u32S >>= 1) {
		uRx = (u32X & u32S) ? 1u : 0u;
		uRy = (u32Y & u32S) ? 1u : 0u;
		u32D += u32S * u32S * ((3u * uRx) ^ uRy);
		b1346_rot(u32S, &u32X, &u32Y, uRx, uRy);
	}
	return u32D;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hilbert2_encode_simple(uint16_t u16X, uint16_t u16Y)
    __attribute__((alias("gj_hilbert2_encode_simple")));
