/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3522: ascending insertion sort of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   void gj_sort_u64_insertion_u(uint64_t *a, size_t n);
 *     - Sort a[0..n) ascending (nondecreasing) in place via insertion
 *       sort. a == NULL or n < 2 → no-op. Stable for equal keys.
 *   void __gj_sort_u64_insertion_u  (alias)
 *   __libcgj_batch3522_marker = "libcgj-batch3522"
 *
 * Exclusive sort algorithms wave (3521-3530). Companion to
 * gj_sort_u32_insertion_u (batch3521). Unique _u name; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3522_marker[] = "libcgj-batch3522";

/* ---- freestanding helpers ---------------------------------------------- */

/* Insertion sort a[0..n); pA non-NULL, cN >= 2. Stable. */
static void
b3522_insertion(uint64_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		uint64_t uKey = pA[i];
		size_t j = i;

		while (j > 0u && pA[j - 1u] > uKey) {
			pA[j] = pA[j - 1u];
			j--;
		}
		pA[j] = uKey;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sort_u64_insertion_u - ascending insertion sort of a[0..n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (empty / singleton already ordered)
 *   equal keys remain in relative order (stable)
 */
void
gj_sort_u64_insertion_u(uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b3522_insertion(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sort_u64_insertion_u(uint64_t *pA, size_t cN)
    __attribute__((alias("gj_sort_u64_insertion_u")));
