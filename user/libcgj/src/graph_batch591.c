/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch591: minimal bit width of a uint64_t.
 *
 * Surface (unique symbols):
 *   unsigned gj_bitwidth_u64(uint64_t x);
 *     — Smallest number of bits needed to represent x.
 *       0 → 0, 1 → 1, 0xFF → 8, UINT64_MAX → 64.
 *   unsigned __gj_bitwidth_u64  (alias)
 *   __libcgj_batch591_marker = "libcgj-batch591"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch591_marker[] = "libcgj-batch591";

/*
 * gj_bitwidth_u64 — floor(log2(x)) + 1 for x > 0; 0 when x == 0.
 *
 * x: value to measure
 *
 * Binary-search peel of 32/16/8/4/2/1 against high half-masks to find
 * the highest set bit, then return position + 1. Zero is defined as 0.
 */
unsigned
gj_bitwidth_u64(uint64_t x)
{
	unsigned uClz;
	uint64_t u64V;

	if (x == 0ull) {
		return 0u;
	}

	u64V = x;
	uClz = 0u;

	if ((u64V & 0xffffffff00000000ull) == 0ull) {
		uClz += 32u;
		u64V <<= 32;
	}
	if ((u64V & 0xffff000000000000ull) == 0ull) {
		uClz += 16u;
		u64V <<= 16;
	}
	if ((u64V & 0xff00000000000000ull) == 0ull) {
		uClz += 8u;
		u64V <<= 8;
	}
	if ((u64V & 0xf000000000000000ull) == 0ull) {
		uClz += 4u;
		u64V <<= 4;
	}
	if ((u64V & 0xc000000000000000ull) == 0ull) {
		uClz += 2u;
		u64V <<= 2;
	}
	if ((u64V & 0x8000000000000000ull) == 0ull) {
		uClz += 1u;
	}

	return 64u - uClz;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_bitwidth_u64(uint64_t x)
    __attribute__((alias("gj_bitwidth_u64")));
