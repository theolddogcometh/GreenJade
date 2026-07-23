/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4039: freestanding uint64_t bit compact (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_compact_bits_u(uint64_t x);
 *     - Gather the even bits of x into the low 32 bits of the result
 *       (bit 2i -> bit i). Odd bits of x are discarded. Inverse of
 *       gj_u64_spread_bits_u for values produced by that expand.
 *       gj_u64_compact_bits_u(0x5555555555555555) == 0xffffffff,
 *       gj_u64_compact_bits_u(0x1) == 0x1,
 *       gj_u64_compact_bits_u(0x5) == 0x3.
 *   uint64_t __gj_u64_compact_bits_u  (alias)
 *   __libcgj_batch4039_marker = "libcgj-batch4039"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u64_compact_bits_u surface only; no multi-def. Distinct from
 * gj_u32_compact_bits_u (batch4038), gj_u64_pext_soft_u (batch3422), and
 * gj_u64_spread_bits_u (batch4037). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4039_marker[] = "libcgj-batch4039";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Compact even bits of a 64-bit word into the low 32 bits
 * (bit 2i -> bit i). Magic-mask compact (inverse of part1by1-64).
 */
static uint64_t
b4039_compact(uint64_t u64X)
{
	u64X &= 0x5555555555555555ULL;
	u64X = (u64X | (u64X >> 1)) & 0x3333333333333333ULL;
	u64X = (u64X | (u64X >> 2)) & 0x0f0f0f0f0f0f0f0fULL;
	u64X = (u64X | (u64X >> 4)) & 0x00ff00ff00ff00ffULL;
	u64X = (u64X | (u64X >> 8)) & 0x0000ffff0000ffffULL;
	u64X = (u64X | (u64X >> 16)) & 0x00000000ffffffffULL;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_compact_bits_u - gather even bits into the low 32 positions.
 *
 * x: source word (odd bits discarded)
 *
 * Returns compacted low-32 word. No parent wires.
 */
uint64_t
gj_u64_compact_bits_u(uint64_t u64X)
{
	(void)NULL;
	return b4039_compact(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_compact_bits_u(uint64_t u64X)
    __attribute__((alias("gj_u64_compact_bits_u")));
