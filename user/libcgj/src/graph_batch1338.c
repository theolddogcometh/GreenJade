/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1338: deinterleave a Morton u32 into two 16-bit halves.
 *
 * Surface (unique symbols):
 *   void gj_u32_deinterleave(uint32_t z, uint16_t *x, uint16_t *y);
 *     — Inverse of gj_u32_interleave: extract even bits into *x and odd
 *       bits into *y. NULL x or NULL y is ignored for that half only.
 *   void __gj_u32_deinterleave  (alias)
 *   __libcgj_batch1338_marker = "libcgj-batch1338"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1338_marker[] = "libcgj-batch1338";

/* ---- freestanding helpers ---------------------------------------------- */

/* Compact even bits of a 32-bit word into the low 16 bits. */
static uint32_t
b1338_compact1by1(uint32_t u)
{
	u &= 0x55555555u;
	u = (u | (u >> 1)) & 0x33333333u;
	u = (u | (u >> 2)) & 0x0f0f0f0fu;
	u = (u | (u >> 4)) & 0x00ff00ffu;
	u = (u | (u >> 8)) & 0x0000ffffu;
	return u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_deinterleave — split interleaved z into even (*x) and odd (*y).
 */
void
gj_u32_deinterleave(uint32_t u32Z, uint16_t *pX, uint16_t *pY)
{
	if (pX != NULL) {
		*pX = (uint16_t)b1338_compact1by1(u32Z);
	}
	if (pY != NULL) {
		*pY = (uint16_t)b1338_compact1by1(u32Z >> 1);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_deinterleave(uint32_t u32Z, uint16_t *pX, uint16_t *pY)
    __attribute__((alias("gj_u32_deinterleave")));
