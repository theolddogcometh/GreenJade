/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2889: in-place unique of sorted uint32_t (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_unique_u32_inplace_u(uint32_t *a, size_t n);
 *     - Compact a[0..n) so consecutive equal runs collapse to one
 *       element, preserving order. Array must already be sorted
 *       nondecreasing. Returns the new length (0..n). NULL a with
 *       n > 0 yields 0; n == 0 yields 0.
 *   size_t __gj_unique_u32_inplace_u  (alias)
 *   __libcgj_batch2889_marker = "libcgj-batch2889"
 *
 * Exclusive sort/search helpers wave (2881-2890). Distinct from
 * gj_uniq_sorted_u32 (batch439 in-place compact) and
 * gj_u32_unique_sorted (batch666 read-only count) — unique _u name;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2889_marker[] = "libcgj-batch2889";

/* ---- freestanding helpers ---------------------------------------------- */

/* Compact sorted run; pA non-NULL, cN >= 1. Returns new length. */
static size_t
b2889_unique(uint32_t *pA, size_t cN)
{
	size_t i;
	size_t cOut;

	cOut = 1u;
	for (i = 1u; i < cN; i++) {
		if (pA[i] != pA[cOut - 1u]) {
			pA[cOut] = pA[i];
			cOut++;
		}
	}
	return cOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unique_u32_inplace_u - in-place unique for a sorted uint32_t range.
 *
 * a: sorted base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns number of unique elements written into a[0..ret). Does not
 * clear trailing slots past ret.
 */
size_t
gj_unique_u32_inplace_u(uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (cN == 0u) {
		return 0u;
	}
	if (pA == NULL) {
		return 0u;
	}
	return b2889_unique(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_unique_u32_inplace_u(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_unique_u32_inplace_u")));
