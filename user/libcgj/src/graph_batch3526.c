/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3526: ascending bubble sort of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   void gj_sort_u64_bubble_u(uint64_t *a, size_t n);
 *     - Sort a[0..n) ascending (nondecreasing) in place via bubble
 *       sort. a == NULL or n < 2 → no-op. Stable for equal keys.
 *   void __gj_sort_u64_bubble_u  (alias)
 *   __libcgj_batch3526_marker = "libcgj-batch3526"
 *
 * Exclusive sort algorithms wave (3521-3530). Companion to
 * gj_sort_u32_bubble_u (batch3525). Unique _u name; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3526_marker[] = "libcgj-batch3526";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3526_swap(uint64_t *pX, uint64_t *pY)
{
	uint64_t uTmp;

	uTmp = *pX;
	*pX = *pY;
	*pY = uTmp;
}

/*
 * Bubble sort a[0..n) with early exit and shrinking bound.
 * pA non-NULL, cN >= 2. Stable.
 */
static void
b3526_bubble(uint64_t *pA, size_t cN)
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
				b3526_swap(&pA[i - 1u], &pA[i]);
				fSwapped = 1;
				cNew = i;
			}
		}
		cBound = cNew;
	} while (fSwapped != 0 && cBound > 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sort_u64_bubble_u - ascending bubble sort of a[0..n).
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (empty / singleton already ordered)
 *   equal keys remain in relative order (stable)
 */
void
gj_sort_u64_bubble_u(uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b3526_bubble(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sort_u64_bubble_u(uint64_t *pA, size_t cN)
    __attribute__((alias("gj_sort_u64_bubble_u")));
