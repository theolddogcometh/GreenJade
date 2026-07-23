/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2239: swap two uint32_t elements by index
 * (post-2230 sort exclusive helper surface).
 *
 * Surface (unique symbols):
 *   void gj_u32_swap_idx(uint32_t *pA, size_t cN, size_t iIdx, size_t jIdx);
 *     - Exchange pA[iIdx] and pA[jIdx] when both indices are in range
 *       [0, cN). NULL pA, cN == 0, iIdx >= cN, jIdx >= cN, or
 *       iIdx == jIdx -> no-op.
 *   void __gj_u32_swap_idx  (alias)
 *   __libcgj_batch2239_marker = "libcgj-batch2239"
 *
 * Distinct from gj_u32_swap_at (batch1009). Unique gj_u32_swap_idx
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2239_marker[] = "libcgj-batch2239";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2239_swap(uint32_t *pX, uint32_t *pY)
{
	uint32_t uTmp;

	uTmp = *pX;
	*pX = *pY;
	*pY = uTmp;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_swap_idx - exchange pA[iIdx] and pA[jIdx] when both are valid.
 *
 * pA:   base of cN uint32_t elements (NULL -> no-op)
 * cN:   element count
 * iIdx: first index
 * jIdx: second index
 *
 * Bounds-checked: if either index is out of range, leaves pA[] unchanged.
 * Does not call libc.
 */
void
gj_u32_swap_idx(uint32_t *pA, size_t cN, size_t iIdx, size_t jIdx)
{
	(void)NULL;
	if (pA == NULL || cN == 0u || iIdx >= cN || jIdx >= cN ||
	    iIdx == jIdx) {
		return;
	}
	b2239_swap(&pA[iIdx], &pA[jIdx]);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_swap_idx(uint32_t *pA, size_t cN, size_t iIdx, size_t jIdx)
    __attribute__((alias("gj_u32_swap_idx")));
