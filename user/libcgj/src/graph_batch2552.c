/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2552: freestanding in-place ascending insertion
 * sort over a bounded int64_t array (sort exclusive wave 2551-2560).
 *
 * Surface (unique symbols):
 *   void gj_i64_insertion_sort_asc(int64_t *pA, size_t cN);
 *     - Sort pA[0..cN) into nondecreasing order in place via insertion
 *       sort. Stable for equal keys. NULL pA -> no-op; cN < 2 -> no-op.
 *   void __gj_i64_insertion_sort_asc  (alias)
 *   __libcgj_batch2552_marker = "libcgj-batch2552"
 *
 * Distinct from gj_u64_insertion_sort_asc (batch2551) and
 * gj_u32_insertion_sort_n (batch2231). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2552_marker[] = "libcgj-batch2552";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2552_insertion_sort(int64_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		int64_t i64Key = pA[i];
		size_t j = i;

		while (j > 0u && pA[j - 1u] > i64Key) {
			pA[j] = pA[j - 1u];
			j--;
		}
		pA[j] = i64Key;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_insertion_sort_asc - in-place ascending insertion sort of cN i64s.
 *
 * pA: base of cN int64_t elements (NULL -> no-op)
 * cN: element count (cN < 2 -> no-op)
 *
 * Stable for equal keys. Mutates pA[0..cN) only. Does not call libc.
 */
void
gj_i64_insertion_sort_asc(int64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN < 2u) {
		return;
	}
	b2552_insertion_sort(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i64_insertion_sort_asc(int64_t *pA, size_t cN)
    __attribute__((alias("gj_i64_insertion_sort_asc")));
