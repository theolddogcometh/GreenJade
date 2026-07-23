/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1005: stable partition of a uint32_t array by
 * pivot (elements strictly less than pivot first).
 *
 * Surface (unique symbols):
 *   size_t gj_u32_stable_partition(uint32_t *a, size_t n, uint32_t pivot);
 *     — Move elements with a[i] < pivot to the front and the rest after,
 *       preserving relative order within each group (stable). In-place
 *       using an O(n) stack buffer of at most 256 uint32_t. Returns the
 *       count of true-side (a[i] < pivot) elements. If a is NULL, n is 0,
 *       or n > 256, returns 0 and leaves the array unchanged (when a is
 *       non-NULL).
 *   size_t __gj_u32_stable_partition  (alias)
 *   __libcgj_batch1005_marker = "libcgj-batch1005"
 *
 * Distinct from gj_stable_partition_nn (batch248, int32_t non-negative).
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1005_marker[] = "libcgj-batch1005";

/* Max n for O(n) stack aux (false-side buffer). */
#define B1005_MAX_N 256u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1005_stable_partition — core: compact a[i] < pivot to a[0..), buffer
 * the rest, then append. Returns true-side count.
 * Caller guarantees a != NULL and 0 < n <= B1005_MAX_N.
 */
static size_t
b1005_stable_partition(uint32_t *a, size_t n, uint32_t uPivot)
{
	uint32_t aFalse[B1005_MAX_N];
	size_t i;
	size_t cTrue = 0u;
	size_t cFalse = 0u;

	for (i = 0u; i < n; i++) {
		uint32_t uV = a[i];

		if (uV < uPivot) {
			a[cTrue] = uV;
			cTrue++;
		} else {
			aFalse[cFalse] = uV;
			cFalse++;
		}
	}

	for (i = 0u; i < cFalse; i++) {
		a[cTrue + i] = aFalse[i];
	}

	return cTrue;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_stable_partition — stable partition: a[i] < pivot first.
 *
 *   a[i] <  pivot → true side  (front), relative order preserved
 *   a[i] >= pivot → false side (back),  relative order preserved
 *
 * Returns count of true-side elements. On NULL a, n == 0, or n > 256:
 * returns 0 and does not modify the array (when a is valid).
 */
size_t
gj_u32_stable_partition(uint32_t *a, size_t n, uint32_t pivot)
{
	if (a == NULL || n == 0u || n > B1005_MAX_N) {
		return 0u;
	}
	return b1005_stable_partition(a, n, pivot);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_stable_partition(uint32_t *a, size_t n, uint32_t pivot)
    __attribute__((alias("gj_u32_stable_partition")));
