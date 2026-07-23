/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1243: bitwise AND-reduce of a uint64_t array.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_and_reduce(const uint64_t *a, size_t n);
 *     — a[0] & a[1] & ... & a[n-1]. n==0 → all-ones (identity).
 *       a==NULL with n>0 → 0.
 *   uint64_t __gj_u64_and_reduce  (alias)
 *   __libcgj_batch1243_marker = "libcgj-batch1243"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1243_marker[] = "libcgj-batch1243";

/*
 * gj_u64_and_reduce — fold array with bitwise AND (identity ~0).
 */
uint64_t
gj_u64_and_reduce(const uint64_t *pA, size_t cN)
{
	uint64_t u64Acc;
	size_t i;

	if (cN == 0u) {
		return ~0ull;
	}
	if (pA == NULL) {
		return 0ull;
	}

	u64Acc = ~0ull;
	for (i = 0u; i < cN; i++) {
		u64Acc &= pA[i];
	}
	return u64Acc;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_and_reduce(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_u64_and_reduce")));
