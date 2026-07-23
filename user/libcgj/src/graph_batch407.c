/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch407: binary search on sorted uint32_t array.
 *
 * Surface (unique symbols):
 *   size_t gj_bsearch_u32(const uint32_t *a, size_t n, uint32_t v);
 *     — Binary-search a sorted nondecreasing a[0..n) for v. Return any
 *       index i with a[i] == v, or n if not found (also when a == NULL
 *       or n == 0). Caller must ensure a is sorted ascending.
 *   size_t __gj_bsearch_u32  (alias)
 *   __libcgj_batch407_marker = "libcgj-batch407"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch407_marker[] = "libcgj-batch407";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bsearch_u32 — index of v in sorted a[0..n), or n if absent.
 *
 * Edge cases:
 *   a == NULL → n
 *   n == 0 → 0 (not-found sentinel is n)
 *   multiple matches → any matching index (classic bsearch)
 */
size_t
gj_bsearch_u32(const uint32_t *a, size_t n, uint32_t v)
{
	size_t lo;
	size_t hi;

	if (a == NULL || n == 0u) {
		return n;
	}

	lo = 0u;
	hi = n;
	while (lo < hi) {
		size_t mid = lo + (hi - lo) / 2u;

		if (a[mid] < v) {
			lo = mid + 1u;
		} else if (a[mid] > v) {
			hi = mid;
		} else {
			return mid;
		}
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bsearch_u32(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_bsearch_u32")));
