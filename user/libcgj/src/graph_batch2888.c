/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2888: test nondecreasing uint64_t order (_u).
 *
 * Surface (unique symbols):
 *   int gj_is_sorted_u64_u(const uint64_t *a, size_t n);
 *     - Return 1 if a[0..n) is sorted nondecreasing (a[i] <= a[i+1]),
 *       else 0. a == NULL with n > 1 → 0; n <= 1 → 1 (vacuously sorted).
 *       Equal adjacent elements allowed (nondecreasing, not strict).
 *   int __gj_is_sorted_u64_u  (alias)
 *   __libcgj_batch2888_marker = "libcgj-batch2888"
 *
 * Exclusive sort/search helpers wave (2881-2890). Companion to
 * gj_is_sorted_u32_u (batch2887). Unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2888_marker[] = "libcgj-batch2888";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nondecreasing. pA non-NULL, cN >= 2. */
static int
b2888_is_sorted(const uint64_t *pA, size_t cN)
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
 * gj_is_sorted_u64_u - 1 if nondecreasing, else 0.
 *
 * Edge cases:
 *   n <= 1 → 1 (empty / singleton is sorted)
 *   a == NULL && n > 1 → 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 */
int
gj_is_sorted_u64_u(const uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (cN <= 1u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	return b2888_is_sorted(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_sorted_u64_u(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_is_sorted_u64_u")));
