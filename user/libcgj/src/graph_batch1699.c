/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1699: segment-tree storage size (power-of-two).
 *
 * Surface (unique symbols):
 *   size_t gj_segment_tree_size(size_t n);
 *     — Array length for a classic 1-based segment tree over n leaves:
 *       2 * next_power_of_two(n). n == 0 → 0. If next_power_of_two(n)
 *       would not fit in size_t, or 2 * that value overflows → 0.
 *   size_t __gj_segment_tree_size  (alias)
 *   __libcgj_batch1699_marker = "libcgj-batch1699"
 *
 * Tree/index exclusive wave (1691–1699). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1699_marker[] = "libcgj-batch1699";

/* Largest power of two in uint64_t: 2^63. */
#define B1699_POW2_63 ((uint64_t)1 << 63)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Smallest power of two >= n as uint64_t. n == 0 → 0.
 * n > 2^63 → 0 (next power would be 2^64).
 */
static uint64_t
b1699_next_pow2_u64(uint64_t u64N)
{
	uint64_t u64X;

	if (u64N == 0ull) {
		return 0ull;
	}
	if (u64N > B1699_POW2_63) {
		return 0ull;
	}

	u64X = u64N - 1ull;
	u64X |= u64X >> 1;
	u64X |= u64X >> 2;
	u64X |= u64X >> 4;
	u64X |= u64X >> 8;
	u64X |= u64X >> 16;
	u64X |= u64X >> 32;
	u64X++;
	return u64X;
}

/* 2 * next_pow2(n), overflow or unrepresentable in size_t → 0. */
static size_t
b1699_seg_size(size_t cN)
{
	uint64_t u64Pow;
	uint64_t u64Slots;

	u64Pow = b1699_next_pow2_u64((uint64_t)cN);
	if (u64Pow == 0ull) {
		return 0u;
	}
	if (u64Pow > (UINT64_MAX / 2ull)) {
		return 0u;
	}
	u64Slots = u64Pow * 2ull;
	if (u64Slots > (uint64_t)(size_t)-1) {
		return 0u;
	}
	return (size_t)u64Slots;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_segment_tree_size — storage slots for a power-of-two segment tree.
 *
 * cN: number of leaves (logical elements)
 * Returns 2 * ceil_pow2(cN), or 0 if cN is 0 or the product overflows.
 */
size_t
gj_segment_tree_size(size_t cN)
{
	return b1699_seg_size(cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_segment_tree_size(size_t cN)
    __attribute__((alias("gj_segment_tree_size")));
