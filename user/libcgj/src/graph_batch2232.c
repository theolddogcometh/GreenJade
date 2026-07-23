/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2232: freestanding in-place ascending insertion
 * sort over a bounded uint64_t array (post-2230 sort exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_u64_insertion_sort_n(uint64_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via insertion
 *       sort. Stable for equal keys. NULL pA -> no-op; cN < 2 -> no-op.
 *   void __gj_u64_insertion_sort_n  (alias)
 *   __libcgj_batch2232_marker = "libcgj-batch2232"
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

const char __libcgj_batch2232_marker[] = "libcgj-batch2232";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2232_insertion_sort(uint64_t *pA, size_t cN)
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
 * gj_u64_insertion_sort_n - in-place ascending insertion sort of cN qwords.
 *
 * pA: base of cN uint64_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Stable for equal keys. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_u64_insertion_sort_n(uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2232_insertion_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u64_insertion_sort_n(uint64_t *pA, size_t cN)
    __attribute__((alias("gj_u64_insertion_sort_n")));
