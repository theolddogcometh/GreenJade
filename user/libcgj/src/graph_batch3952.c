/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3952: freestanding uint64 full bit reverse (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bit_reverse_u(uint64_t x);
 *     - Reverse bit order of a 64-bit value (bit 0 <-> bit 63, ...).
 *       SWAR parallel reverse; pure integer, no builtins or tables.
 *   uint64_t __gj_u64_bit_reverse_u  (alias)
 *   __libcgj_batch3952_marker = "libcgj-batch3952"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u64_bit_reverse_u surface only; no multi-def. Distinct from
 * gj_u64_bit_reverse (batch2134) and gj_bitrev64 (batch181).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3952_marker[] = "libcgj-batch3952";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR reverse of all 64 bits: stages 1, 2, 4, 8, 16, then word swap. */
static uint64_t
b3952_bitrev(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V = ((u64V >> 1) & 0x5555555555555555ull)
	     | ((u64V & 0x5555555555555555ull) << 1);
	u64V = ((u64V >> 2) & 0x3333333333333333ull)
	     | ((u64V & 0x3333333333333333ull) << 2);
	u64V = ((u64V >> 4) & 0x0f0f0f0f0f0f0f0full)
	     | ((u64V & 0x0f0f0f0f0f0f0f0full) << 4);
	u64V = ((u64V >> 8) & 0x00ff00ff00ff00ffull)
	     | ((u64V & 0x00ff00ff00ff00ffull) << 8);
	u64V = ((u64V >> 16) & 0x0000ffff0000ffffull)
	     | ((u64V & 0x0000ffff0000ffffull) << 16);
	u64V = (u64V >> 32) | (u64V << 32);
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bit_reverse_u - reverse the bit order of a 64-bit word.
 *
 * x: value whose bits are to be reversed
 *
 * Example: 0x1 -> high bit set; high bit alone -> 0x1.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_bit_reverse_u(uint64_t u64X)
{
	(void)NULL;
	return b3952_bitrev(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bit_reverse_u(uint64_t u64X)
    __attribute__((alias("gj_u64_bit_reverse_u")));
