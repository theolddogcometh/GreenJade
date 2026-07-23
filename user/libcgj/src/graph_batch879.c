/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch879: bitset word-array equality.
 *
 * Surface (unique symbols):
 *   int gj_bitset_eq(const uint64_t *a, const uint64_t *b, size_t nwords);
 *     — 1 if a and b match over nwords words. nwords==0 → 1.
 *       both NULL → 1; exactly one NULL and nwords>0 → 0.
 *   int __gj_bitset_eq  (alias)
 *   __libcgj_batch879_marker = "libcgj-batch879"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch879_marker[] = "libcgj-batch879";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_eq — word-wise equality of two dense bitsets.
 */
int
gj_bitset_eq(const uint64_t *pA, const uint64_t *pB, size_t cNwords)
{
	size_t i;

	if (cNwords == 0u) {
		return 1;
	}
	if (pA == NULL && pB == NULL) {
		return 1;
	}
	if (pA == NULL || pB == NULL) {
		return 0;
	}
	if (pA == pB) {
		return 1;
	}
	for (i = 0u; i < cNwords; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset_eq(const uint64_t *pA, const uint64_t *pB, size_t cNwords)
    __attribute__((alias("gj_bitset_eq")));
