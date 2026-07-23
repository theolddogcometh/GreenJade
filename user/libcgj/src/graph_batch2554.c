/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2554: freestanding in-place ascending cocktail
 * (bidirectional bubble) sort over a bounded uint32_t array (sort exclusive
 * wave 2551-2560).
 *
 * Surface (unique symbols):
 *   void gj_u32_cocktail_sort_asc(uint32_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via cocktail
 *       sort. NULL pA -> no-op; cN < 2 -> no-op.
 *   void __gj_u32_cocktail_sort_asc  (alias)
 *   __libcgj_batch2554_marker = "libcgj-batch2554"
 *
 * Distinct from gj_u32_shell_sort_asc (batch2553) and insertion sorts.
 * Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2554_marker[] = "libcgj-batch2554";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2554_swap(uint32_t *pA, uint32_t *pB)
{
	uint32_t u32Tmp;

	u32Tmp = *pA;
	*pA = *pB;
	*pB = u32Tmp;
}

static void
b2554_cocktail_sort(uint32_t *pA, size_t cN)
{
	size_t iStart = 0u;
	size_t iEnd = cN - 1u;
	int fSwapped = 1;

	while (fSwapped != 0 && iStart < iEnd) {
		size_t i;

		fSwapped = 0;
		for (i = iStart; i < iEnd; i++) {
			if (pA[i] > pA[i + 1u]) {
				b2554_swap(&pA[i], &pA[i + 1u]);
				fSwapped = 1;
			}
		}
		if (fSwapped == 0) {
			break;
		}
		iEnd--;

		fSwapped = 0;
		i = iEnd;
		while (i > iStart) {
			if (pA[i - 1u] > pA[i]) {
				b2554_swap(&pA[i - 1u], &pA[i]);
				fSwapped = 1;
			}
			i--;
		}
		iStart++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cocktail_sort_asc - in-place ascending cocktail sort of cN words.
 *
 * pA: base of cN uint32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Bidirectional bubble: forward pass sinks max, reverse pass floats min.
 * Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u32_cocktail_sort_asc(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2554_cocktail_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_cocktail_sort_asc(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_cocktail_sort_asc")));
