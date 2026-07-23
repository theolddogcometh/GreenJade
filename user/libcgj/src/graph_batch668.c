/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch668: upper-bound on sorted uint32_t array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_upper_bound(const uint32_t *a, size_t n, uint32_t v);
 *     — First index i in [0, n] such that a[i] > v (or n if every
 *       element is <= v). a[0..n) must be sorted nondecreasing.
 *       n == 0 returns 0. NULL a with n > 0 returns n.
 *   size_t __gj_u32_upper_bound  (alias)
 *   __libcgj_batch668_marker = "libcgj-batch668"
 *
 * Distinct from gj_bsearch_u32 / __gj_bsearch_u32 (graph_batch407 exact
 * match) and from gj_u32_find / __gj_u32_find (graph_batch611 linear
 * scan). Unique gj_ names only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch668_marker[] = "libcgj-batch668";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_upper_bound — first index i with a[i] > v (or n if none).
 *
 * a: sorted nondecreasing array of n uint32_t (NULL with n > 0 → n)
 * n: element count (0 → 0)
 * v: probe value
 *
 * Classic binary upper-bound: elements in [0, result) are all <= v;
 * elements in [result, n) are all > v. Does not set errno.
 */
size_t
gj_u32_upper_bound(const uint32_t *a, size_t n, uint32_t v)
{
	size_t lo;
	size_t hi;

	if (n == 0u || a == NULL) {
		return n;
	}

	lo = 0u;
	hi = n;
	while (lo < hi) {
		size_t mid = lo + (hi - lo) / 2u;

		if (a[mid] <= v) {
			lo = mid + 1u;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_upper_bound(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_upper_bound")));
