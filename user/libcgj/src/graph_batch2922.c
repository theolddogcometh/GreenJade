/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2922: freestanding 64-bit bit reverse (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bitrev64_u(uint64_t x);
 *     - Reverse all 64 bits of x (LSB becomes MSB and vice versa).
 *   uint64_t __gj_bitrev64_u  (alias)
 *   __libcgj_batch2922_marker = "libcgj-batch2922"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_bitrev64 (batch181) — unique gj_bitrev64_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2922_marker[] = "libcgj-batch2922";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2922_rev64 - SWAR pairwise bit-group swaps (1, 2, 4, 8, 16, 32).
 */
static uint64_t
b2922_rev64(uint64_t u64X)
{
	u64X = ((u64X >> 1) & 0x5555555555555555ull) |
	       ((u64X & 0x5555555555555555ull) << 1);
	u64X = ((u64X >> 2) & 0x3333333333333333ull) |
	       ((u64X & 0x3333333333333333ull) << 2);
	u64X = ((u64X >> 4) & 0x0f0f0f0f0f0f0f0full) |
	       ((u64X & 0x0f0f0f0f0f0f0f0full) << 4);
	u64X = ((u64X >> 8) & 0x00ff00ff00ff00ffull) |
	       ((u64X & 0x00ff00ff00ff00ffull) << 8);
	u64X = ((u64X >> 16) & 0x0000ffff0000ffffull) |
	       ((u64X & 0x0000ffff0000ffffull) << 16);
	u64X = (u64X >> 32) | (u64X << 32);
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitrev64_u - reverse the bit order of a 64-bit word.
 * Example: 0x1ull → 0x8000000000000000ull.
 */
uint64_t
gj_bitrev64_u(uint64_t u64X)
{
	(void)NULL;
	return b2922_rev64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bitrev64_u(uint64_t u64X)
    __attribute__((alias("gj_bitrev64_u")));
