/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3439: bit-deinterleave one u32 into two u16 (_u).
 *
 * Surface (unique symbols):
 *   void gj_u32_bit_deinterleave_u(uint32_t z, uint16_t *a, uint16_t *b);
 *     - Inverse of gj_u32_bit_interleave_u: even bits -> *a, odd bits -> *b.
 *       NULL out-pointers are ignored (partial write allowed).
 *   void __gj_u32_bit_deinterleave_u  (alias)
 *   __libcgj_batch3439_marker = "libcgj-batch3439"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u32_bit_deinterleave_u surface only; no multi-def. Distinct from
 * gj_bit_deinterleave_u32 (batch1343), gj_u32_deinterleave (batch1338),
 * gj_morton2_decode (batch1345). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3439_marker[] = "libcgj-batch3439";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Compact even bits of a 32-bit word into the low 16 bits (inverse of
 * part1by1 expand). Magic-mask compact.
 */
static uint32_t
b3439_compact1by1(uint32_t u32X)
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
 * gj_u32_bit_deinterleave_u - unzip a 32-bit interleaved word into two u16.
 *
 * u32Z: interleaved source (even -> a, odd -> b)
 * pA:   optional even-bit destination
 * pB:   optional odd-bit destination
 *
 * Does not call libc. No parent wires.
 */
void
gj_u32_bit_deinterleave_u(uint32_t u32Z, uint16_t *pA, uint16_t *pB)
{
	if (pA != NULL) {
		*pA = (uint16_t)b3439_compact1by1(u32Z);
	}
	if (pB != NULL) {
		*pB = (uint16_t)b3439_compact1by1(u32Z >> 1);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_bit_deinterleave_u(uint32_t u32Z, uint16_t *pA, uint16_t *pB)
    __attribute__((alias("gj_u32_bit_deinterleave_u")));
