/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch439: freestanding in-place unique of a
 * non-decreasing sorted uint32_t array.
 *
 * Surface (unique symbols):
 *   size_t gj_uniq_sorted_u32(uint32_t *a, size_t n);
 *     — Compact a[0..n) so that consecutive equal runs collapse to one
 *       element, preserving order. Array must already be sorted
 *       non-decreasing. Returns the new length (0..n). NULL a with
 *       n > 0 yields 0; n == 0 yields 0.
 *   size_t __gj_uniq_sorted_u32  (alias)
 *   __libcgj_batch439_marker = "libcgj-batch439"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch439_marker[] = "libcgj-batch439";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uniq_sorted_u32 — in-place unique for a sorted uint32_t range.
 *
 * a: sorted base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns number of unique elements written into a[0..ret).
 */
size_t
gj_uniq_sorted_u32(uint32_t *pA, size_t cN)
{
	size_t i;
	size_t cOut;

	if (cN == 0u) {
		return 0u;
	}
	if (pA == NULL) {
		return 0u;
	}

	cOut = 1u;
	for (i = 1u; i < cN; i++) {
		if (pA[i] != pA[cOut - 1u]) {
			pA[cOut] = pA[i];
			cOut++;
		}
	}
	return cOut;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_uniq_sorted_u32(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_uniq_sorted_u32")));
