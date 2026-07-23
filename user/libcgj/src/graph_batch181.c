/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch181: full-width bit reverse (32/64).
 * Clean-room freestanding pure C — integer only (builds under userspace
 * -msse2; this TU does not require SSE). No third-party bit-reverse
 * source was copied.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bitrev32(uint32_t x);
 *     — reverse all 32 bits of x (LSB becomes MSB and vice versa).
 *   uint64_t gj_bitrev64(uint64_t x);
 *     — reverse all 64 bits of x (LSB becomes MSB and vice versa).
 *   __gj_bitrev32 / __gj_bitrev64  (aliases)
 *   __libcgj_batch181_marker = "libcgj-batch181"
 *
 * Algorithm: parallel SWAR pairwise swaps of adjacent bit groups
 * (1, 2, 4, 8, 16[, 32]) using constant masks — O(log width) steps.
 */

#include <stdint.h>

const char __libcgj_batch181_marker[] = "libcgj-batch181";

/* ---- gj_bitrev32 ------------------------------------------------------- */

/*
 * gj_bitrev32 — reverse the bit order of a 32-bit word.
 * Example: 0x00000001u → 0x80000000u; 0x12345678u → 0x1e6a2c48u.
 */
uint32_t
gj_bitrev32(uint32_t x)
{
	x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
	x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
	x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
	x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
	x = (x >> 16) | (x << 16);
	return x;
}

uint32_t __gj_bitrev32(uint32_t x)
    __attribute__((alias("gj_bitrev32")));

/* ---- gj_bitrev64 ------------------------------------------------------- */

/*
 * gj_bitrev64 — reverse the bit order of a 64-bit word.
 * Example: 0x1ull → 0x8000000000000000ull.
 */
uint64_t
gj_bitrev64(uint64_t x)
{
	x = ((x >> 1) & 0x5555555555555555ull) | ((x & 0x5555555555555555ull) << 1);
	x = ((x >> 2) & 0x3333333333333333ull) | ((x & 0x3333333333333333ull) << 2);
	x = ((x >> 4) & 0x0f0f0f0f0f0f0f0full) | ((x & 0x0f0f0f0f0f0f0f0full) << 4);
	x = ((x >> 8) & 0x00ff00ff00ff00ffull) | ((x & 0x00ff00ff00ff00ffull) << 8);
	x = ((x >> 16) & 0x0000ffff0000ffffull) | ((x & 0x0000ffff0000ffffull) << 16);
	x = (x >> 32) | (x << 32);
	return x;
}

uint64_t __gj_bitrev64(uint64_t x)
    __attribute__((alias("gj_bitrev64")));
