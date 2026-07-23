/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1241: population count over a uint64_t array.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_popcnt_arr(const uint64_t *a, size_t n);
 *     — Sum of Hamming weights of a[0..n). n==0 or a==NULL → 0.
 *   uint64_t __gj_u64_popcnt_arr  (alias)
 *   __libcgj_batch1241_marker = "libcgj-batch1241"
 *
 * Distinct from gj_popcnt64 / gj_popcnt_u64 (scalar) — array surface.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1241_marker[] = "libcgj-batch1241";

/* SWAR popcnt of one word (no builtins — freestanding safe). */
static unsigned
b1241_popcnt64(uint64_t u64X)
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

/*
 * gj_u64_popcnt_arr — total set-bit count across n words.
 */
uint64_t
gj_u64_popcnt_arr(const uint64_t *pA, size_t cN)
{
	uint64_t u64Total;
	size_t i;

	if (pA == NULL || cN == 0u) {
		return 0ull;
	}

	u64Total = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Total += (uint64_t)b1241_popcnt64(pA[i]);
	}
	return u64Total;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_popcnt_arr(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_u64_popcnt_arr")));
