/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch562: population count (Hamming weight) of a
 * uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_popcnt_u64(uint64_t x);
 *     — Number of 1-bits in x (0 .. 64). x == 0 yields 0.
 *   unsigned __gj_popcnt_u64  (alias)
 *   __libcgj_batch562_marker = "libcgj-batch562"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch562_marker[] = "libcgj-batch562";

/*
 * gj_popcnt_u64 — count set bits in a 64-bit word (SWAR parallel sum).
 *
 * x: value to inspect
 *
 * Classic mask/add tree: 2-bit → 4-bit → 8-bit groups, then multiply-
 * fold by 0x01010101... and take the high byte.
 */
unsigned
gj_popcnt_u64(uint64_t x)
{
	uint64_t v;

	v = x;
	v = v - ((v >> 1) & 0x5555555555555555ull);
	v = (v & 0x3333333333333333ull) + ((v >> 2) & 0x3333333333333333ull);
	v = (v + (v >> 4)) & 0x0f0f0f0f0f0f0f0full;
	v = (v * 0x0101010101010101ull) >> 56;

	return (unsigned)v;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_popcnt_u64(uint64_t x)
    __attribute__((alias("gj_popcnt_u64")));
