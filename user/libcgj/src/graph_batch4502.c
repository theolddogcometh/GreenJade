/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4502: reverse all 64 bits of a uint64_t word.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_rev_bits_u(uint64_t x);
 *     - Reverse bit order of x (bit 0 <-> bit 63, ...). Pure integer SWAR;
 *       no builtins, no __int128.
 *   uint64_t __gj_u64_rev_bits_u  (alias)
 *   __libcgj_batch4502_marker = "libcgj-batch4502"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from gj_bitrev64
 * (batch181) — unique gj_u64_rev_bits_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4502_marker[] = "libcgj-batch4502";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4502_rev_bits(uint64_t u64X)
{
	u64X = ((u64X >> 1) & 0x5555555555555555ULL)
	     | ((u64X & 0x5555555555555555ULL) << 1);
	u64X = ((u64X >> 2) & 0x3333333333333333ULL)
	     | ((u64X & 0x3333333333333333ULL) << 2);
	u64X = ((u64X >> 4) & 0x0F0F0F0F0F0F0F0FULL)
	     | ((u64X & 0x0F0F0F0F0F0F0F0FULL) << 4);
	u64X = ((u64X >> 8) & 0x00FF00FF00FF00FFULL)
	     | ((u64X & 0x00FF00FF00FF00FFULL) << 8);
	u64X = ((u64X >> 16) & 0x0000FFFF0000FFFFULL)
	     | ((u64X & 0x0000FFFF0000FFFFULL) << 16);
	u64X = (u64X >> 32) | (u64X << 32);
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_rev_bits_u - reverse the bit order of a 64-bit word.
 *
 * x: value whose bits are to be reversed
 *
 * Returns x with bits mirrored end-to-end. Example:
 * gj_u64_rev_bits_u(1ULL) -> 0x8000000000000000ULL.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_rev_bits_u(uint64_t u64X)
{
	(void)NULL;
	return b4502_rev_bits(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_rev_bits_u(uint64_t u64X)
    __attribute__((alias("gj_u64_rev_bits_u")));
