/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4038: freestanding uint32_t bit compact (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_compact_bits_u(uint32_t x);
 *     - Gather the even bits of x into the low 16 bits of the result
 *       (bit 2i -> bit i). Odd bits of x are discarded. Inverse of
 *       gj_u32_spread_bits_u for values produced by that expand.
 *       gj_u32_compact_bits_u(0x55555555) == 0x0000ffff,
 *       gj_u32_compact_bits_u(0x00000001) == 0x00000001,
 *       gj_u32_compact_bits_u(0x00000005) == 0x00000003.
 *   uint32_t __gj_u32_compact_bits_u  (alias)
 *   __libcgj_batch4038_marker = "libcgj-batch4038"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u32_compact_bits_u surface only; no multi-def. Distinct from
 * gj_u32_bit_deinterleave_u (batch3439), gj_u32_pext_soft_u (batch3421),
 * and gj_u32_spread_bits_u (batch4036). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4038_marker[] = "libcgj-batch4038";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Compact even bits of a 32-bit word into the low 16 bits
 * (bit 2i -> bit i). Magic-mask compact (inverse of part1by1).
 */
static uint32_t
b4038_compact(uint32_t u32X)
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
 * gj_u32_compact_bits_u - gather even bits into the low 16 positions.
 *
 * x: source word (odd bits discarded)
 *
 * Returns compacted low-16 word. No parent wires.
 */
uint32_t
gj_u32_compact_bits_u(uint32_t u32X)
{
	(void)NULL;
	return b4038_compact(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_compact_bits_u(uint32_t u32X)
    __attribute__((alias("gj_u32_compact_bits_u")));
