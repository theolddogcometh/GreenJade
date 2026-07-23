/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch408: ascending insertion sort of int64_t.
 *
 * Surface (unique symbols):
 *   void gj_sort_i64(int64_t *a, size_t n);
 *     — Sort a[0..n) ascending (nondecreasing) in place via insertion
 *       sort. a == NULL or n < 2 → no-op. Stable for equal keys.
 *   void __gj_sort_i64  (alias)
 *   __libcgj_batch408_marker = "libcgj-batch408"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch408_marker[] = "libcgj-batch408";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sort_i64 — ascending insertion sort of a[0..n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (empty / singleton already ordered)
 *   equal keys remain in relative order (stable)
 */
void
gj_sort_i64(int64_t *a, size_t n)
{
	size_t i;

	if (a == NULL || n < 2u) {
		return;
	}

	for (i = 1u; i < n; i++) {
		int64_t i64Key = a[i];
		size_t j = i;

		while (j > 0u && a[j - 1u] > i64Key) {
			a[j] = a[j - 1u];
			j--;
		}
		a[j] = i64Key;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sort_i64(int64_t *a, size_t n)
    __attribute__((alias("gj_sort_i64")));
