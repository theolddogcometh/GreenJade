/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch248: stable partition of int32_t array by
 * fixed predicate (non-negative first).
 *
 * Surface (unique symbols):
 *   size_t gj_stable_partition_nn(int32_t *a, size_t n);
 *     — Move elements with a[i] >= 0 to the front, negatives after,
 *       preserving relative order within each group (stable). In-place
 *       using an O(n) stack buffer of at most 256 int32_t. Returns the
 *       count of non-negative elements. If a is NULL, n is 0, or n > 256,
 *       returns 0 and leaves the array unchanged (when a is non-NULL).
 *   __gj_stable_partition_nn  (alias)
 *   __libcgj_batch248_marker = "libcgj-batch248"
 *
 * Fixed predicate (no function pointers): keep non-negative first
 * (x >= 0). Clean-room freestanding pure C (integer/pointer only).
 * Compiles with -ffreestanding -msse2. No malloc, no errno. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch248_marker[] = "libcgj-batch248";

/* Max n for O(n) stack aux (false-side buffer). */
#define B248_MAX_N 256u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b248_stable_partition_nn — core: compact non-negatives to a[0..),
 * buffer negatives, then append. Returns non-negative count.
 * Caller guarantees a != NULL and 0 < n <= B248_MAX_N.
 */
static size_t
b248_stable_partition_nn(int32_t *a, size_t n)
{
	int32_t aFalse[B248_MAX_N];
	size_t i;
	size_t nTrue = 0u;
	size_t nFalse = 0u;

	for (i = 0u; i < n; i++) {
		int32_t v = a[i];

		if (v >= 0) {
			a[nTrue] = v;
			nTrue++;
		} else {
			aFalse[nFalse] = v;
			nFalse++;
		}
	}

	for (i = 0u; i < nFalse; i++) {
		a[nTrue + i] = aFalse[i];
	}

	return nTrue;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stable_partition_nn — stable partition: non-negative first.
 *
 *   a[i] >= 0  → true side  (front), relative order preserved
 *   a[i] <  0  → false side (back),  relative order preserved
 *
 * Returns count of true-side elements. On NULL a, n == 0, or n > 256:
 * returns 0 and does not modify the array (when a is valid).
 *
 * Stack aux is O(n) with hard cap n <= 256 (false-side buffer).
 */
size_t
gj_stable_partition_nn(int32_t *a, size_t n)
{
	if (a == NULL || n == 0u || n > B248_MAX_N) {
		return 0u;
	}
	return b248_stable_partition_nn(a, n);
}

size_t __gj_stable_partition_nn(int32_t *a, size_t n)
    __attribute__((alias("gj_stable_partition_nn")));
