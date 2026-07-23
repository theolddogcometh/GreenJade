/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch669: freestanding in-place ascending insertion
 * sort over a bounded uint32_t array.
 *
 * Surface (unique symbols):
 *   void gj_u32_insertion_sort(uint32_t *a, size_t n);
 *     — Sort a[0..n) into nondecreasing (ascending) order in place via
 *       insertion sort. Stable for equal keys. NULL a → no-op; n < 2
 *       → no-op.
 *   void __gj_u32_insertion_sort  (alias)
 *   __libcgj_batch669_marker = "libcgj-batch669"
 *
 * Distinct from gj_qsort / gj_heapsort / gj_mergesort (batch100) and from
 * gj_u32_is_sorted_asc (batch665 predicate only). Unique gj_ names only;
 * no multi-def with the existing sort graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch669_marker[] = "libcgj-batch669";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_insertion_sort — in-place ascending insertion sort of n words.
 *
 * a: base of n uint32_t elements (NULL → no-op)
 * n: element count (n < 2 → no-op)
 *
 * For i = 1..n-1, extract key = a[i] and shift a[j] one slot right while
 * a[j] > key (j walking left), then place key at the insertion hole.
 * Equal elements are not reordered past one another (stable). Mutates
 * a[0..n) only. Does not call libc qsort or other sort surfaces.
 */
void
gj_u32_insertion_sort(uint32_t *a, size_t n)
{
	size_t i;

	if (a == NULL || n < 2u) {
		return;
	}

	for (i = 1u; i < n; i++) {
		uint32_t key = a[i];
		size_t j = i;

		while (j > 0u && a[j - 1u] > key) {
			a[j] = a[j - 1u];
			j--;
		}
		a[j] = key;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_insertion_sort(uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_insertion_sort")));
