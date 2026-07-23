/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch872: strictly unique sorted uint32 check.
 *
 * Surface (unique symbols):
 *   int gj_u32_is_unique_sorted(const uint32_t *a, size_t n);
 *     — 1 if a[0..n) is strictly increasing (a[i] < a[i+1]). Empty → 1.
 *       NULL a with n>0 → 0. Non-strict / unsorted → 0.
 *   int __gj_u32_is_unique_sorted  (alias)
 *   __libcgj_batch872_marker = "libcgj-batch872"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch872_marker[] = "libcgj-batch872";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_unique_sorted — 1 iff sorted ascending with no duplicates.
 *
 * Requires a[i] < a[i+1] for all i. n <= 1 is always unique when a is
 * non-NULL (or n==0).
 */
int
gj_u32_is_unique_sorted(const uint32_t *pA, size_t cN)
{
	size_t i;

	if (cN == 0u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	if (cN == 1u) {
		return 1;
	}

	for (i = 1u; i < cN; i++) {
		if (pA[i] <= pA[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_unique_sorted(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_is_unique_sorted")));
