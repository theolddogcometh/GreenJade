/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2235: freestanding in-place ascending bubble
 * sort over a bounded uint32_t array (post-2230 sort exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_u32_bubble_sort_n(uint32_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via bubble sort.
 *       Stable for equal keys. NULL pA -> no-op; cN < 2 -> no-op.
 *   void __gj_u32_bubble_sort_n  (alias)
 *   __libcgj_batch2235_marker = "libcgj-batch2235"
 *
 * Distinct from gj_u32_insertion_sort_n (batch2231) and
 * gj_u32_selection_sort_n (batch2234). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2235_marker[] = "libcgj-batch2235";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2235_swap(uint32_t *pX, uint32_t *pY)
{
	uint32_t uTmp;

	uTmp = *pX;
	*pX = *pY;
	*pY = uTmp;
}

static void
b2235_bubble_sort(uint32_t *pA, size_t cN)
{
	size_t cBound;
	int fSwapped;

	cBound = cN;
	do {
		size_t i;
		size_t cNew;

		fSwapped = 0;
		cNew = 0u;
		for (i = 1u; i < cBound; i++) {
			if (pA[i - 1u] > pA[i]) {
				b2235_swap(&pA[i - 1u], &pA[i]);
				fSwapped = 1;
				cNew = i;
			}
		}
		cBound = cNew;
	} while (fSwapped != 0 && cBound > 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bubble_sort_n - in-place ascending bubble sort of cN words.
 *
 * pA: base of cN uint32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Adjacent-swap bubble with early exit and shrinking bound. Stable for
 * equal keys. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u32_bubble_sort_n(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2235_bubble_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_bubble_sort_n(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_bubble_sort_n")));
