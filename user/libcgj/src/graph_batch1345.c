/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1345: 2D Morton (Z-order) decode.
 *
 * Surface (unique symbols):
 *   void gj_morton2_decode(uint32_t code, uint16_t *x, uint16_t *y);
 *     — Inverse of gj_morton2_encode. NULL out-pointers ignored.
 *   void __gj_morton2_decode  (alias)
 *   __libcgj_batch1345_marker = "libcgj-batch1345"
 *
 * Distinct from gj_bit_deinterleave_u32 (batch1343). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1345_marker[] = "libcgj-batch1345";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1345_compact1by1(uint32_t u32X)
{
	u32X &= 0x55555555u;
	u32X = (u32X | (u32X >> 1)) & 0x33333333u;
	u32X = (u32X | (u32X >> 2)) & 0x0f0f0f0fu;
	u32X = (u32X | (u32X >> 4)) & 0x00ff00ffu;
	u32X = (u32X | (u32X >> 8)) & 0x0000ffffu;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_morton2_decode — recover (x, y) from a 2D Morton code.
 *
 * u32Code: Morton / Z-order index
 * pX:      optional X destination (even bits)
 * pY:      optional Y destination (odd bits)
 */
void
gj_morton2_decode(uint32_t u32Code, uint16_t *pX, uint16_t *pY)
{
	if (pX != NULL) {
		*pX = (uint16_t)b1345_compact1by1(u32Code);
	}
	if (pY != NULL) {
		*pY = (uint16_t)b1345_compact1by1(u32Code >> 1);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_morton2_decode(uint32_t u32Code, uint16_t *pX, uint16_t *pY)
    __attribute__((alias("gj_morton2_decode")));
