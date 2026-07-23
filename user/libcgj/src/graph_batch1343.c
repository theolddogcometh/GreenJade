/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1343: bit-deinterleave one u32 into two u16.
 *
 * Surface (unique symbols):
 *   void gj_bit_deinterleave_u32(uint32_t z, uint16_t *a, uint16_t *b);
 *     — Inverse of gj_bit_interleave_u16: even bits → *a, odd bits → *b.
 *       NULL out-pointers are ignored (partial write allowed).
 *   void __gj_bit_deinterleave_u32  (alias)
 *   __libcgj_batch1343_marker = "libcgj-batch1343"
 *
 * Distinct from gj_morton2_decode (batch1345). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1343_marker[] = "libcgj-batch1343";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Compact even bits of a 32-bit word into the low 16 bits (inverse of
 * part1by1 expand). Magic-mask compact.
 */
static uint32_t
b1343_compact1by1(uint32_t u32X)
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
 * gj_bit_deinterleave_u32 — unzip a 32-bit interleaved word into two u16.
 *
 * u32Z: interleaved source (even → a, odd → b)
 * pA:   optional even-bit destination
 * pB:   optional odd-bit destination
 */
void
gj_bit_deinterleave_u32(uint32_t u32Z, uint16_t *pA, uint16_t *pB)
{
	if (pA != NULL) {
		*pA = (uint16_t)b1343_compact1by1(u32Z);
	}
	if (pB != NULL) {
		*pB = (uint16_t)b1343_compact1by1(u32Z >> 1);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bit_deinterleave_u32(uint32_t u32Z, uint16_t *pA, uint16_t *pB)
    __attribute__((alias("gj_bit_deinterleave_u32")));
