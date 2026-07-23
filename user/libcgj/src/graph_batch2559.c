/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2559: freestanding ascending sort-order check
 * over a bounded uint32_t array (sort exclusive wave 2551-2560).
 *
 * Surface (unique symbols):
 *   int gj_u32_sort_check_asc(const uint32_t *pA, size_t cN);
 *     - Return 1 (OK) if pA[0..cN) is sorted nondecreasing
 *       (pA[i] <= pA[i+1]), else 0 (FAIL). pA == NULL with cN > 1 -> 0;
 *       cN <= 1 -> 1 (vacuously sorted). Equal adjacent elements allowed.
 *   int __gj_u32_sort_check_asc  (alias)
 *   __libcgj_batch2559_marker = "libcgj-batch2559"
 *
 * Distinct from gj_u32_is_sorted_asc (batch665) and
 * gj_u32_is_sorted_asc_n (batch2236). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2559_marker[] = "libcgj-batch2559";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2559_sort_check_asc(const uint32_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		if (pA[i] < pA[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sort_check_asc - 1 (OK) if nondecreasing, else 0 (FAIL).
 *
 * Edge cases:
 *   cN <= 1 -> 1 (empty / singleton is sorted)
 *   pA == NULL && cN > 1 -> 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 */
int
gj_u32_sort_check_asc(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (cN <= 1u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	return b2559_sort_check_asc(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_sort_check_asc(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_sort_check_asc")));
