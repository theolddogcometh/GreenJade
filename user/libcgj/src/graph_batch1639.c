/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1639: u64 bitset subset predicate.
 *
 * Surface (unique symbols):
 *   int gj_bitset_u64_subset_p(const uint64_t *sub, const uint64_t *super,
 *                              size_t nwords);
 *     — 1 if every set bit of sub is also set in super over nwords words
 *       (i.e. (sub[i] & ~super[i]) == 0 for all i). nwords == 0 → 1.
 *       both NULL → 1; exactly one NULL and nwords > 0 → 0.
 *   int __gj_bitset_u64_subset_p  (alias)
 *   __libcgj_batch1639_marker = "libcgj-batch1639"
 *
 * Distinct from gj_bitset_eq / gj_bitset_any / flags32_test_all —
 * unique multi-word subset predicate.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1639_marker[] = "libcgj-batch1639";

/* ---- freestanding helpers ---------------------------------------------- */

/* True iff every bit of pSub is present in pSuper over cNwords. */
static int
b1639_subset_p(const uint64_t *pSub, const uint64_t *pSuper, size_t cNwords)
{
	size_t i;

	for (i = 0u; i < cNwords; i++) {
		if ((pSub[i] & ~pSuper[i]) != 0ull) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_u64_subset_p — 1 if sub is a subset of super (word-wise).
 *
 * sub: candidate subset word array.
 * super: candidate superset word array.
 * nwords: number of uint64_t words to compare.
 *
 * Empty domain (nwords == 0) is vacuously true. Both NULL with nwords
 * > 0 is treated as equal empty sources → true; mixed NULL → false.
 */
int
gj_bitset_u64_subset_p(const uint64_t *pSub, const uint64_t *pSuper,
    size_t cNwords)
{
	if (cNwords == 0u) {
		return 1;
	}
	if (pSub == NULL && pSuper == NULL) {
		return 1;
	}
	if (pSub == NULL || pSuper == NULL) {
		return 0;
	}
	if (pSub == pSuper) {
		return 1;
	}
	return b1639_subset_p(pSub, pSuper, cNwords);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset_u64_subset_p(const uint64_t *pSub, const uint64_t *pSuper,
    size_t cNwords)
    __attribute__((alias("gj_bitset_u64_subset_p")));
