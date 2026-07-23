/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch867: uint16_t array first min index.
 *
 * Surface (unique symbols):
 *   size_t gj_u16_find_min(const uint16_t *a, size_t n);
 *     — Index of first min element in a[0..n). Ties keep the
 *       smallest index. n==0 or NULL a → n.
 *   size_t __gj_u16_find_min  (alias)
 *   __libcgj_batch867_marker = "libcgj-batch867"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch867_marker[] = "libcgj-batch867";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_find_min — first index of the min value in a[0..n).
 *
 * Linear scan. On ties, the lowest index wins. Empty or NULL → n.
 */
size_t
gj_u16_find_min(const uint16_t *pA, size_t cN)
{
	size_t iBest;
	size_t i;
	uint16_t vBest;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	iBest = 0u;
	vBest = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] < vBest) {
			vBest = pA[i];
			iBest = i;
		}
	}
	return iBest;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u16_find_min(const uint16_t *pA, size_t cN)
    __attribute__((alias("gj_u16_find_min")));
