/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2231: freestanding in-place ascending insertion
 * sort over a bounded uint32_t array (post-2230 sort exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_u32_insertion_sort_n(uint32_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via insertion
 *       sort. Stable for equal keys. NULL pA -> no-op; cN < 2 -> no-op.
 *   void __gj_u32_insertion_sort_n  (alias)
 *   __libcgj_batch2231_marker = "libcgj-batch2231"
 *
 * Distinct from gj_u32_insertion_sort (batch669). Unique gj_ names only;
 * no multi-def with prior sort surfaces.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2231_marker[] = "libcgj-batch2231";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2231_insertion_sort(uint32_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		uint32_t uKey = pA[i];
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
 * gj_u32_insertion_sort_n - in-place ascending insertion sort of cN words.
 *
 * pA: base of cN uint32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Stable for equal keys. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u32_insertion_sort_n(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2231_insertion_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_insertion_sort_n(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_insertion_sort_n")));
