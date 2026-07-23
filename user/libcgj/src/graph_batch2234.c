/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2234: freestanding in-place ascending selection
 * sort over a bounded uint32_t array (post-2230 sort exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_u32_selection_sort_n(uint32_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via selection
 *       sort. NULL pA -> no-op; cN < 2 -> no-op. Not stable.
 *   void __gj_u32_selection_sort_n  (alias)
 *   __libcgj_batch2234_marker = "libcgj-batch2234"
 *
 * Distinct from gj_u32_insertion_sort (batch669), gj_u32_insertion_sort_n
 * (batch2231), and gj_u32_select_nth (batch712). Unique gj_ names only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2234_marker[] = "libcgj-batch2234";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2234_swap(uint32_t *pX, uint32_t *pY)
{
	uint32_t uTmp;

	uTmp = *pX;
	*pX = *pY;
	*pY = uTmp;
}

static void
b2234_selection_sort(uint32_t *pA, size_t cN)
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
			b2234_swap(&pA[i], &pA[iMin]);
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_selection_sort_n - in-place ascending selection sort of cN words.
 *
 * pA: base of cN uint32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * For each prefix position i, finds the minimum of pA[i..cN) and swaps it
 * into pA[i]. Not stable. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u32_selection_sort_n(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2234_selection_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_selection_sort_n(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_selection_sort_n")));
