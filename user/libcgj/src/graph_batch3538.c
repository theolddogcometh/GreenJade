/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3538: Lomuto partition of a u64 array about
 * an external pivot (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_arr_u64_partition_u(uint64_t *a, size_t n, uint64_t pivot);
 *     - Rearrange a[0..n) so every element < pivot is left of every
 *       element >= pivot. Returns the boundary index i where a[0..i)
 *       are all < pivot and a[i..n) are all >= pivot. NULL a or
 *       n == 0 → 0 (no-op).
 *   size_t __gj_arr_u64_partition_u  (alias)
 *   __libcgj_batch3538_marker = "libcgj-batch3538"
 *
 * Exclusive continuum CREATE-ONLY (3531-3540). Companion to
 * gj_arr_u32_partition_u (batch3537). Unique arr_u64_partition_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3538_marker[] = "libcgj-batch3538";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Lomuto scan: pack elements < pivot to the left.
 * pA non-NULL, cN > 0. Returns count of elements < pivot.
 */
static size_t
b3538_part(uint64_t *pA, size_t cN, uint64_t uPivot)
{
	size_t i;
	size_t j;

	i = 0u;
	for (j = 0u; j < cN; j++) {
		if (pA[j] < uPivot) {
			if (i != j) {
				uint64_t uT = pA[i];

				pA[i] = pA[j];
				pA[j] = uT;
			}
			i++;
		}
	}
	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_partition_u - partition a[0..n) about pivot.
 *
 * a:     base of n uint64_t (NULL → 0, no-op)
 * n:     element count (0 → 0)
 * pivot: partition key (need not appear in a)
 *
 * Mutates a[0..n) only. Relative order among equals is not guaranteed.
 * Does not call libc.
 */
size_t
gj_arr_u64_partition_u(uint64_t *pA, size_t cN, uint64_t uPivot)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b3538_part(pA, cN, uPivot);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arr_u64_partition_u(uint64_t *pA, size_t cN, uint64_t uPivot)
    __attribute__((alias("gj_arr_u64_partition_u")));
