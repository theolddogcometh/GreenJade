/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2236: test nondecreasing uint32_t order (asc)
 * post-2230 sort exclusive predicate surface.
 *
 * Surface (unique symbols):
 *   int gj_u32_is_sorted_asc_n(const uint32_t *pA, size_t cN);
 *     - Return 1 (OK) if pA[0..cN) is sorted nondecreasing
 *       (pA[i] <= pA[i+1]), else 0 (FAIL). pA == NULL with cN > 1 -> 0;
 *       cN <= 1 -> 1 (vacuously sorted). Equal adjacent elements allowed.
 *   int __gj_u32_is_sorted_asc_n  (alias)
 *   __libcgj_batch2236_marker = "libcgj-batch2236"
 *
 * Distinct from gj_u32_is_sorted_asc (batch665) and gj_is_sorted_u32
 * (batch405). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2236_marker[] = "libcgj-batch2236";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2236_is_sorted_asc(const uint32_t *pA, size_t cN)
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
 * gj_u32_is_sorted_asc_n - 1 (OK) if nondecreasing, else 0 (FAIL).
 *
 * Edge cases:
 *   cN <= 1 -> 1 (empty / singleton is sorted)
 *   pA == NULL && cN > 1 -> 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 */
int
gj_u32_is_sorted_asc_n(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (cN <= 1u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	return b2236_is_sorted_asc(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_sorted_asc_n(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_is_sorted_asc_n")));
