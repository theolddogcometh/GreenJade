/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2233: freestanding in-place ascending insertion
 * sort over a bounded int32_t array (post-2230 sort exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_i32_insertion_sort_n(int32_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via insertion
 *       sort (signed compare). Stable for equal keys. NULL pA -> no-op;
 *       cN < 2 -> no-op.
 *   void __gj_i32_insertion_sort_n  (alias)
 *   __libcgj_batch2233_marker = "libcgj-batch2233"
 *
 * Distinct from gj_u32_insertion_sort (batch669) and
 * gj_u32_insertion_sort_n (batch2231). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2233_marker[] = "libcgj-batch2233";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2233_insertion_sort(int32_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		int32_t iKey = pA[i];
		size_t j = i;

		while (j > 0u && pA[j - 1u] > iKey) {
			pA[j] = pA[j - 1u];
			j--;
		}
		pA[j] = iKey;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_insertion_sort_n - in-place ascending insertion sort of cN dwords.
 *
 * pA: base of cN int32_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Uses signed comparison so negatives sort before positives. Stable for
 * equal keys. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_i32_insertion_sort_n(int32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2233_insertion_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_insertion_sort_n(int32_t *pA, size_t cN)
    __attribute__((alias("gj_i32_insertion_sort_n")));
