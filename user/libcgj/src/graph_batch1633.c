/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1633: isolate lowest set bit in a u64 bitset.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bitset64_isolate_lsb(uint64_t x);
 *     — Return x & -x: the single lowest set bit of x, or 0 if x == 0.
 *       Pure integer two's-complement isolate; no builtins.
 *   uint64_t __gj_bitset64_isolate_lsb  (alias)
 *   __libcgj_batch1633_marker = "libcgj-batch1633"
 *
 * Distinct from gj_isolate_lsb (batch420) and gj_isolate_lsb_u64
 * (batch599) — unique bitset64 surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stdint.h>

const char __libcgj_batch1633_marker[] = "libcgj-batch1633";

/* ---- freestanding helpers ---------------------------------------------- */

/* Two's-complement isolate of the least-significant 1-bit. */
static uint64_t
b1633_isolate_lsb(uint64_t u64X)
{
	return u64X & (uint64_t)(0ull - u64X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_isolate_lsb — keep only the least-significant 1-bit of x.
 *
 * x: dense 64-bit bitset (bit 0 = LSB).
 *
 * Examples: 0 → 0; 0x10 → 0x10; 0x12 → 0x2; ~0ull → 1.
 */
uint64_t
gj_bitset64_isolate_lsb(uint64_t u64X)
{
	return b1633_isolate_lsb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bitset64_isolate_lsb(uint64_t u64X)
    __attribute__((alias("gj_bitset64_isolate_lsb")));
