/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2555: freestanding max-heap predicate over a
 * bounded uint32_t array (sort exclusive wave 2551-2560).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_heap_max_p(const uint32_t *pA, size_t cN);
 *     - Return 1 (OK) if pA[0..cN) is a binary max-heap (parent >= both
 *       children), else 0 (FAIL). pA == NULL with cN > 1 -> 0; cN <= 1
 *       -> 1 (vacuously a heap).
 *   int __gj_u32_is_heap_max_p  (alias)
 *   __libcgj_batch2555_marker = "libcgj-batch2555"
 *
 * Layout: 0-based; children of i are 2i+1 and 2i+2. Distinct from
 * gj_u32_is_heap_min_p (batch2556) and sift helpers. Unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2555_marker[] = "libcgj-batch2555";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2555_is_heap_max(const uint32_t *pA, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		size_t iLeft = 2u * i + 1u;
		size_t iRight = 2u * i + 2u;

		if (iLeft < cN && pA[i] < pA[iLeft]) {
			return 0;
		}
		if (iRight < cN && pA[i] < pA[iRight]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_heap_max_p - 1 (OK) if max-heap, else 0 (FAIL).
 *
 * Edge cases:
 *   cN <= 1 -> 1 (empty / singleton is a heap)
 *   pA == NULL && cN > 1 -> 0
 *   parent >= children (equal keys allowed)
 */
int
gj_u32_is_heap_max_p(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (cN <= 1u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	return b2555_is_heap_max(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_heap_max_p(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_is_heap_max_p")));
