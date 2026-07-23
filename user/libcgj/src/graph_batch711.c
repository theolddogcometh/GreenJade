/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch711: freestanding Lomuto partition of a
 * bounded uint32_t array around an external pivot value.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_partition(uint32_t *a, size_t n, uint32_t pivot);
 *     — Rearrange a[0..n) so every element < pivot is left of every
 *       element >= pivot (Lomuto-style single scan). Returns the
 *       boundary index i where a[0..i) are all < pivot and a[i..n)
 *       are all >= pivot. NULL a or n == 0 → 0 (no-op).
 *   size_t __gj_u32_partition  (alias)
 *   __libcgj_batch711_marker = "libcgj-batch711"
 *
 * Distinct from gj_stable_partition_nn (batch248, int32_t non-negative
 * predicate) and from internal Lomuto helpers in gj_qsort (batch100).
 * Unique gj_u32_partition names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch711_marker[] = "libcgj-batch711";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_partition — Lomuto partition of a[0..n) about pivot.
 *
 * a:     base of n uint32_t elements (NULL → return 0, no-op)
 * n:     element count (0 → return 0)
 * pivot: partition key (not required to appear in a)
 *
 * Walk j from 0 to n-1. When a[j] < pivot, swap a[j] with a[i] and
 * advance the write index i. On exit, a[0..i) hold all strict-less
 * elements (relative order among equals not guaranteed) and a[i..n)
 * hold elements >= pivot. Returns i (count of elements < pivot).
 *
 * Mutates a[0..n) only. Does not call qsort, heapsort, or other graph
 * sort/partition surfaces.
 */
size_t
gj_u32_partition(uint32_t *a, size_t n, uint32_t pivot)
{
	size_t i;
	size_t j;

	if (a == NULL || n == 0u) {
		return 0u;
	}

	i = 0u;
	for (j = 0u; j < n; j++) {
		if (a[j] < pivot) {
			if (i != j) {
				uint32_t t = a[i];

				a[i] = a[j];
				a[j] = t;
			}
			i++;
		}
	}

	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_partition(uint32_t *a, size_t n, uint32_t pivot)
    __attribute__((alias("gj_u32_partition")));
