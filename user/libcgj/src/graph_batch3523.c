/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3523: ascending selection sort of uint32_t (_u).
 *
 * Surface (unique symbols):
 *   void gj_sort_u32_selection_u(uint32_t *a, size_t n);
 *     - Sort a[0..n) ascending (nondecreasing) in place via selection
 *       sort. a == NULL or n < 2 → no-op. Not stable.
 *   void __gj_sort_u32_selection_u  (alias)
 *   __libcgj_batch3523_marker = "libcgj-batch3523"
 *
 * Exclusive sort algorithms wave (3521-3530). Distinct from
 * gj_u32_selection_sort_n (batch2234) — unique _u name; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3523_marker[] = "libcgj-batch3523";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3523_swap(uint32_t *pX, uint32_t *pY)
{
	uint32_t uTmp;

	uTmp = *pX;
	*pX = *pY;
	*pY = uTmp;
}

/* Selection sort a[0..n); pA non-NULL, cN >= 2. Not stable. */
static void
b3523_selection(uint32_t *pA, size_t cN)
{
	size_t i;

	for (i = 0u; i + 1u < cN; i++) {
		size_t iMin = i;
		size_t j;

		for (j = i + 1u; j < cN; j++) {
			if (pA[j] < pA[iMin]) {
				iMin = j;
			}
		}
		if (iMin != i) {
			b3523_swap(&pA[i], &pA[iMin]);
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sort_u32_selection_u - ascending selection sort of a[0..n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (empty / singleton already ordered)
 *   not stable for equal keys
 */
void
gj_sort_u32_selection_u(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b3523_selection(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sort_u32_selection_u(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_sort_u32_selection_u")));
