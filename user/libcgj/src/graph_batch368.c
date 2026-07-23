/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch368: population count (Hamming weight) of a
 * uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_popcnt64(uint64_t x);
 *     — Number of 1-bits in x (0 .. 64). x == 0 yields 0.
 *   unsigned __gj_popcnt64  (alias)
 *   __libcgj_batch368_marker = "libcgj-batch368"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch368_marker[] = "libcgj-batch368";

/*
 * gj_popcnt64 — count set bits in a 64-bit word (SWAR parallel sum).
 *
 * x: value to inspect
 *
 * Classic mask/add tree: 2-bit → 4-bit → 8-bit groups, then multiply-
 * fold by 0x01010101... and take the high byte.
 */
unsigned
gj_popcnt64(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V = u64V - ((u64V >> 1) & 0x5555555555555555ull);
	u64V = (u64V & 0x3333333333333333ull) +
	    ((u64V >> 2) & 0x3333333333333333ull);
	u64V = (u64V + (u64V >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64V = (u64V * 0x0101010101010101ull) >> 56;

	return (unsigned)u64V;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_popcnt64(uint64_t u64X)
    __attribute__((alias("gj_popcnt64")));
