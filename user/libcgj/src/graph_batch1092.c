/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1092: total Hamming weight of a uint64 array
 *
 * Surface (unique symbols):
 *   size_t gj_hamming_weight_arr_u64(const uint64_t *a, size_t n);
 *     — Sum of popcount(a[i]) for i in [0, n). NULL a or n == 0 → 0.
 *       SWAR popcount only (no __builtin_popcount dependency).
 *   size_t __gj_hamming_weight_arr_u64  (alias)

 *   __libcgj_batch1092_marker = "libcgj-batch1092"
 *
 * Distinct from gj_popcount64 / gj_popcount_buf / gj_hamming_* — unique name.
 * * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1092_marker[] = "libcgj-batch1092";

static unsigned
b1092_pop64(uint64_t u64X)
{
	u64X = u64X - ((u64X >> 1) & 0x5555555555555555ull);
	u64X = (u64X & 0x3333333333333333ull) +
	       ((u64X >> 2) & 0x3333333333333333ull);
	u64X = (u64X + (u64X >> 4)) & 0x0f0f0f0f0f0f0f0full;
	u64X = u64X + (u64X >> 8);
	u64X = u64X + (u64X >> 16);
	u64X = u64X + (u64X >> 32);
	return (unsigned)(u64X & 0x7full);
}

/*
 * gj_hamming_weight_arr_u64 — total set-bit count across n limbs.
 */
size_t
gj_hamming_weight_arr_u64(const uint64_t *pA, size_t cN)
{
	size_t i;
	size_t cTotal;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}

	cTotal = 0u;
	for (i = 0u; i < cN; i++) {
		cTotal += (size_t)b1092_pop64(pA[i]);
	}
	return cTotal;
}

size_t __gj_hamming_weight_arr_u64(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_hamming_weight_arr_u64")));
