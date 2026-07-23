/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4036: freestanding uint32_t bit spread (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_spread_bits_u(uint32_t x);
 *     - Spread the low 16 bits of x into the even bit positions of a
 *       32-bit word (bit i -> bit 2i). High 16 bits of x are ignored.
 *       Classic part1by1 expand; inverse is gj_u32_compact_bits_u.
 *       gj_u32_spread_bits_u(0x0000ffff) == 0x55555555,
 *       gj_u32_spread_bits_u(0x00000001) == 0x00000001,
 *       gj_u32_spread_bits_u(0x00000003) == 0x00000005.
 *   uint32_t __gj_u32_spread_bits_u  (alias)
 *   __libcgj_batch4036_marker = "libcgj-batch4036"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u32_spread_bits_u surface only; no multi-def. Distinct from
 * gj_u32_bit_interleave_u (batch3438, two-source zip), gj_u32_interleave
 * (batch1337), and gj_u32_pdep_soft_u (batch3418, mask deposit).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4036_marker[] = "libcgj-batch4036";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Spread low 16 bits of u32X into even positions (bit i -> bit 2i).
 * Magic-mask expand (part1by1).
 */
static uint32_t
b4036_spread(uint32_t u32X)
{
	u32X &= 0x0000ffffu;
	u32X = (u32X | (u32X << 8)) & 0x00ff00ffu;
	u32X = (u32X | (u32X << 4)) & 0x0f0f0f0fu;
	u32X = (u32X | (u32X << 2)) & 0x33333333u;
	u32X = (u32X | (u32X << 1)) & 0x55555555u;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_spread_bits_u - spread low 16 bits into even positions.
 *
 * x: source word (only bits 0..15 matter)
 *
 * Returns expanded word with zeros in odd positions. No parent wires.
 */
uint32_t
gj_u32_spread_bits_u(uint32_t u32X)
{
	(void)NULL;
	return b4036_spread(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_spread_bits_u(uint32_t u32X)
    __attribute__((alias("gj_u32_spread_bits_u")));
