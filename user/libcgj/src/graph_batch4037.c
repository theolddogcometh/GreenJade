/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4037: freestanding uint64_t bit spread (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_spread_bits_u(uint64_t x);
 *     - Spread the low 32 bits of x into the even bit positions of a
 *       64-bit word (bit i -> bit 2i). High 32 bits of x are ignored.
 *       Classic part1by1 expand; inverse is gj_u64_compact_bits_u.
 *       gj_u64_spread_bits_u(0xffffffff) == 0x5555555555555555,
 *       gj_u64_spread_bits_u(0x1) == 0x1,
 *       gj_u64_spread_bits_u(0x3) == 0x5.
 *   uint64_t __gj_u64_spread_bits_u  (alias)
 *   __libcgj_batch4037_marker = "libcgj-batch4037"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u64_spread_bits_u surface only; no multi-def. Distinct from
 * gj_u32_spread_bits_u (batch4036), gj_u64_pdep_soft_u (batch3419), and
 * Morton interleave helpers. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4037_marker[] = "libcgj-batch4037";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Spread low 32 bits of u64X into even positions (bit i -> bit 2i).
 * Magic-mask expand (part1by1 for 64-bit).
 */
static uint64_t
b4037_spread(uint64_t u64X)
{
	u64X &= 0x00000000ffffffffULL;
	u64X = (u64X | (u64X << 16)) & 0x0000ffff0000ffffULL;
	u64X = (u64X | (u64X << 8)) & 0x00ff00ff00ff00ffULL;
	u64X = (u64X | (u64X << 4)) & 0x0f0f0f0f0f0f0f0fULL;
	u64X = (u64X | (u64X << 2)) & 0x3333333333333333ULL;
	u64X = (u64X | (u64X << 1)) & 0x5555555555555555ULL;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_spread_bits_u - spread low 32 bits into even positions.
 *
 * x: source word (only bits 0..31 matter)
 *
 * Returns expanded word with zeros in odd positions. No parent wires.
 */
uint64_t
gj_u64_spread_bits_u(uint64_t u64X)
{
	(void)NULL;
	return b4037_spread(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_spread_bits_u(uint64_t u64X)
    __attribute__((alias("gj_u64_spread_bits_u")));
