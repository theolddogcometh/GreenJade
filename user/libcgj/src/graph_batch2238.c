/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2238: freestanding Lomuto partition of a
 * bounded uint32_t array around the last element as pivot (post-2230
 * sort exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_u32_partition_lomuto(uint32_t *pA, size_t cN);
 *     - Rearrange pA[0..cN) so elements <= pivot end left of elements
 *       > pivot, with pivot = pA[cN-1] placed at its final index p.
 *       Returns p. NULL pA or cN == 0 -> 0 (no-op); cN == 1 -> 0.
 *   size_t __gj_u32_partition_lomuto  (alias)
 *   __libcgj_batch2238_marker = "libcgj-batch2238"
 *
 * Distinct from gj_u32_partition (batch711 external pivot) and from
 * internal Lomuto helpers in gj_u32_select_nth (batch712). Unique
 * gj_u32_partition_lomuto names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2238_marker[] = "libcgj-batch2238";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2238_swap(uint32_t *pX, uint32_t *pY)
{
	uint32_t uTmp;

	uTmp = *pX;
	*pX = *pY;
	*pY = uTmp;
}

/*
 * Classic Lomuto partition of pA[0..cN) about pivot pA[cN-1].
 * Returns final pivot index p: pA[0..p) all <= pivot, pA[p] is pivot,
 * pA[p+1..cN) all > pivot.
 */
static size_t
b2238_partition_lomuto(uint32_t *pA, size_t cN)
{
	uint32_t uPivot;
	size_t i;
	size_t j;
	size_t iHi;

	iHi = cN - 1u;
	uPivot = pA[iHi];
	i = 0u;
	for (j = 0u; j < iHi; j++) {
		if (pA[j] <= uPivot) {
			if (i != j) {
				b2238_swap(&pA[i], &pA[j]);
			}
			i++;
		}
	}
	b2238_swap(&pA[i], &pA[iHi]);
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_partition_lomuto - Lomuto partition of pA[0..cN) about pA[cN-1].
 *
 * pA: base of cN uint32_t elements (NULL -> return 0, no-op)
 * cN: element count (0 -> return 0; 1 -> return 0 with no mutation needed)
 *
 * On exit, pA[0..p] hold values <= original pivot, pA[p] is the pivot,
 * and pA[p+1..cN) hold values > pivot. Returns p. Mutates pA[0..cN) only.
 * Does not call qsort or other graph sort/partition surfaces.
 */
size_t
gj_u32_partition_lomuto(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	if (cN == 1u) {
		return 0u;
	}
	return b2238_partition_lomuto(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_partition_lomuto(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_partition_lomuto")));
