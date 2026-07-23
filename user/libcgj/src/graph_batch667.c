/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch667: lower_bound over a sorted uint32_t array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_lower_bound(const uint32_t *a, size_t n, uint32_t v);
 *     — First index i in [0, n] such that a[i] >= v (a sorted ascending).
 *       Returns n if every element is < v, or if n == 0, or if a is NULL
 *       with n > 0. a[0..n) must be non-decreasing.
 *   size_t __gj_u32_lower_bound  (alias)
 *   __libcgj_batch667_marker = "libcgj-batch667"
 *
 * Distinct from gj_u32_find (batch611 linear equality scan) and from
 * b311_lower_bound (private helper for sorted int32 insert/find). Unique
 * gj_ names only; no multi-def with the existing libc graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch667_marker[] = "libcgj-batch667";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lower_bound — first index i with a[i] >= v, or n if none.
 *
 * a: sorted ascending array of n uint32_t (NULL with n > 0 → n)
 * n: element count (0 → 0)
 * v: threshold value
 *
 * Binary search; O(log n) comparisons. Midpoint uses lo + ((hi - lo) >> 1)
 * to avoid overflow on large ranges.
 */
size_t
gj_u32_lower_bound(const uint32_t *a, size_t n, uint32_t v)
{
	size_t lo;
	size_t hi;

	if (n == 0u || a == NULL) {
		return n;
	}

	lo = 0u;
	hi = n;
	while (lo < hi) {
		size_t mid = lo + ((hi - lo) >> 1);

		if (a[mid] < v) {
			lo = mid + 1u;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_lower_bound(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_lower_bound")));
