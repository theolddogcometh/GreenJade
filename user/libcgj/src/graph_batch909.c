/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch909: percentile of a sorted uint32 array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_percentile_sorted(const uint32_t *a, size_t n,
 *                                     unsigned p);
 *     — Nearest-rank percentile: index = (p * (n - 1)) / 100 for
 *       p in [0, 100]. Returns a[index]. NULL a, n == 0, or p > 100 → 0.
 *   uint32_t __gj_u32_percentile_sorted  (alias)
 *   __libcgj_batch909_marker = "libcgj-batch909"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch909_marker[] = "libcgj-batch909";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_percentile_sorted — p-th percentile of sorted uint32 data.
 *
 * p: percentage in [0, 100]. Uses the closed interval rank formula
 *    index = floor(p * (n - 1) / 100)
 * so p == 0 → a[0], p == 100 → a[n-1], p == 50 → lower median for
 * even lengths when n is odd/even accordingly.
 *
 * Multiplication is done in 64-bit to avoid size_t overflow for large n.
 */
uint32_t
gj_u32_percentile_sorted(const uint32_t *a, size_t n, unsigned p)
{
	size_t iIdx;
	uint64_t uProd;

	if (a == NULL || n == 0u || p > 100u) {
		return 0u;
	}

	uProd = (uint64_t)p * (uint64_t)(n - 1u);
	iIdx = (size_t)(uProd / 100ull);
	return a[iIdx];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_percentile_sorted(const uint32_t *a, size_t n, unsigned p)
    __attribute__((alias("gj_u32_percentile_sorted")));
