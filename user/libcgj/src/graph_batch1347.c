/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1347: simple 2D Hilbert-curve decode (16-bit).
 *
 * Surface (unique symbols):
 *   void gj_hilbert2_decode_simple(uint32_t d, uint16_t *x, uint16_t *y);
 *     — Inverse of gj_hilbert2_encode_simple. NULL out-pointers ignored.
 *   void __gj_hilbert2_decode_simple  (alias)
 *   __libcgj_batch1347_marker = "libcgj-batch1347"
 *
 * Distinct from Morton decode (batch1345). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1347_marker[] = "libcgj-batch1347";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1347_rot(uint32_t u32S, uint32_t *pX, uint32_t *pY, uint32_t uRx, uint32_t uRy)
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
 * gj_hilbert2_decode_simple — recover (x, y) from a Hilbert distance.
 *
 * u32D: Hilbert index on a 2^16 square
 * pX:   optional X destination
 * pY:   optional Y destination
 */
void
gj_hilbert2_decode_simple(uint32_t u32D, uint16_t *pX, uint16_t *pY)
{
	uint32_t u32X;
	uint32_t u32Y;
	uint32_t u32S;
	uint32_t u32T;
	uint32_t uRx;
	uint32_t uRy;

	u32X = 0u;
	u32Y = 0u;

	for (u32S = 1u; u32S < 65536u; u32S <<= 1) {
		uRx = 1u & (u32D / 2u);
		uRy = 1u & (u32D ^ uRx);
		b1347_rot(u32S, &u32X, &u32Y, uRx, uRy);
		u32X += u32S * uRx;
		u32Y += u32S * uRy;
		u32D /= 4u;
	}

	/* Silence unused if both outs NULL; still pure compute. */
	u32T = u32X | u32Y;
	(void)u32T;

	if (pX != NULL) {
		*pX = (uint16_t)u32X;
	}
	if (pY != NULL) {
		*pY = (uint16_t)u32Y;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_hilbert2_decode_simple(uint32_t u32D, uint16_t *pX, uint16_t *pY)
    __attribute__((alias("gj_hilbert2_decode_simple")));
