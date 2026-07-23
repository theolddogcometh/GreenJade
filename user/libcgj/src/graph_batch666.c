/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch666: freestanding count of unique values in
 * a non-decreasing sorted uint32_t array (read-only).
 *
 * Surface (unique symbols; distinct from gj_uniq_sorted_u32 /
 * __gj_uniq_sorted_u32 in graph_batch439.c which compact in place —
 * no multi-def):
 *   size_t gj_u32_unique_sorted(const uint32_t *a, size_t n);
 *     — Return how many distinct values appear in a[0..n). Array must
 *       already be sorted non-decreasing (a[i] <= a[i+1]). NULL a with
 *       n > 0 yields 0; n == 0 yields 0. Does not mutate a.
 *   size_t __gj_u32_unique_sorted  (alias)
 *   __libcgj_batch666_marker = "libcgj-batch666"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch666_marker[] = "libcgj-batch666";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_unique_sorted — count distinct values in a sorted run.
 *
 * a: sorted non-decreasing base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Walks a[0..n) once. Each time a[i] differs from a[i-1] (or i == 0),
 * counts a new unique value. Returns the unique count in 0..n.
 */
size_t
gj_u32_unique_sorted(const uint32_t *a, size_t n)
{
	size_t i;
	size_t c;

	if (n == 0u || a == NULL) {
		return 0u;
	}

	c = 1u;
	for (i = 1u; i < n; i++) {
		if (a[i] != a[i - 1u]) {
			c++;
		}
	}
	return c;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_unique_sorted(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_unique_sorted")));
