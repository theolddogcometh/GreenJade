/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch406: ascending insertion sort of uint32_t.
 *
 * Surface (unique symbols):
 *   void gj_sort_u32(uint32_t *a, size_t n);
 *     — Sort a[0..n) ascending (nondecreasing) in place via insertion
 *       sort. a == NULL or n < 2 → no-op. Stable for equal keys.
 *   void __gj_sort_u32  (alias)
 *   __libcgj_batch406_marker = "libcgj-batch406"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch406_marker[] = "libcgj-batch406";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sort_u32 — ascending insertion sort of a[0..n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (empty / singleton already ordered)
 *   equal keys remain in relative order (stable)
 */
void
gj_sort_u32(uint32_t *a, size_t n)
{
	size_t i;

	if (a == NULL || n < 2u) {
		return;
	}

	for (i = 1u; i < n; i++) {
		uint32_t uKey = a[i];
		size_t j = i;

		while (j > 0u && a[j - 1u] > uKey) {
			a[j] = a[j - 1u];
			j--;
		}
		a[j] = uKey;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sort_u32(uint32_t *a, size_t n)
    __attribute__((alias("gj_sort_u32")));
