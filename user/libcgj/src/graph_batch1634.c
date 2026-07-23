/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1634: isolate highest set bit in a u64 bitset.
 *
 * Surface (unique symbols):
 *   uint64_t gj_bitset64_isolate_msb(uint64_t x);
 *     — Return a mask with only the highest set bit of x kept, or 0 if
 *       x == 0. Pure integer smear + isolate; no builtins.
 *   uint64_t __gj_bitset64_isolate_msb  (alias)
 *   __libcgj_batch1634_marker = "libcgj-batch1634"
 *
 * Distinct from gj_bitset64_isolate_lsb (batch1633) and prior isolate_*
 * LSB surfaces — unique bitset64 MSB surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stdint.h>

const char __libcgj_batch1634_marker[] = "libcgj-batch1634";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Isolate most-significant 1-bit: smear bits downward then peel off
 * the lower smear with (smear >> 1). Zero stays zero.
 */
static uint64_t
b1634_isolate_msb(uint64_t u64X)
{
	uint64_t u64V;

	if (u64X == 0ull) {
		return 0ull;
	}

	u64V = u64X;
	u64V |= u64V >> 1;
	u64V |= u64V >> 2;
	u64V |= u64V >> 4;
	u64V |= u64V >> 8;
	u64V |= u64V >> 16;
	u64V |= u64V >> 32;
	return u64V - (u64V >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_isolate_msb — keep only the most-significant 1-bit of x.
 *
 * x: dense 64-bit bitset (bit 0 = LSB, bit 63 = MSB).
 *
 * Examples: 0 → 0; 0x10 → 0x10; 0x12 → 0x10; ~0ull → (1ull << 63).
 */
uint64_t
gj_bitset64_isolate_msb(uint64_t u64X)
{
	return b1634_isolate_msb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bitset64_isolate_msb(uint64_t u64X)
    __attribute__((alias("gj_bitset64_isolate_msb")));
