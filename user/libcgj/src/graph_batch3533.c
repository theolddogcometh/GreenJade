/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3533: count distinct values in a u64 array
 * (unsorted freestanding, _u).
 *
 * Surface (unique symbols):
 *   size_t gj_arr_u64_unique_count_u(const uint64_t *a, size_t n);
 *     - Return how many distinct values appear in a[0..n). Array need
 *       not be sorted. NULL a or n == 0 → 0. Does not mutate a.
 *   size_t __gj_arr_u64_unique_count_u  (alias)
 *   __libcgj_batch3533_marker = "libcgj-batch3533"
 *
 * Exclusive continuum CREATE-ONLY (3531-3540). Companion to
 * gj_arr_u32_unique_count_u (batch3532). Unique arr_u64 unique_count_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3533_marker[] = "libcgj-batch3533";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Count distinct values via O(n^2) first-occurrence scan.
 * pA non-NULL, cN > 0. No allocation.
 */
static size_t
b3533_unique_count(const uint64_t *pA, size_t cN)
{
	size_t i;
	size_t j;
	size_t c;

	c = 0u;
	for (i = 0u; i < cN; i++) {
		int bFirst;

		bFirst = 1;
		for (j = 0u; j < i; j++) {
			if (pA[j] == pA[i]) {
				bFirst = 0;
				break;
			}
		}
		if (bFirst) {
			c++;
		}
	}
	return c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_unique_count_u - count distinct uint64_t values in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Order-independent; does not require sorted input. Does not call libc.
 */
size_t
gj_arr_u64_unique_count_u(const uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b3533_unique_count(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arr_u64_unique_count_u(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u64_unique_count_u")));
