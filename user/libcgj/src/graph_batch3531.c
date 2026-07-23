/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3531: strict ascending order test on u64 (_u).
 *
 * Surface (unique symbols):
 *   int gj_is_sorted_strict_u64_u(const uint64_t *a, size_t n);
 *     - Return 1 if a[0..n) is strictly increasing (a[i] < a[i+1] for
 *       every adjacent pair), else 0. a == NULL with n > 1 → 0;
 *       n <= 1 → 1 (vacuously sorted). Equal adjacent elements fail.
 *   int __gj_is_sorted_strict_u64_u  (alias)
 *   __libcgj_batch3531_marker = "libcgj-batch3531"
 *
 * Exclusive continuum CREATE-ONLY (3531-3540): is_sorted_strict_u64_u,
 * arr_u32_unique_count_u, arr_u64_unique_count_u, arr_u32_mode_u,
 * arr_u32_median_u, arr_u64_median_u, arr_u32_partition_u,
 * arr_u64_partition_u, arr_u32_nth_element_u, batch_id_3540. Distinct
 * from gj_is_sorted_u64_u (batch2888 nondecreasing) and
 * gj_is_sorted_u32_u (batch2887) — unique strict u64 surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3531_marker[] = "libcgj-batch3531";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if strictly increasing. pA non-NULL, cN >= 2. */
static int
b3531_is_sorted_strict(const uint64_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		if (pA[i] <= pA[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_sorted_strict_u64_u - 1 if strictly increasing, else 0.
 *
 * Edge cases:
 *   n <= 1 → 1 (empty / singleton is sorted)
 *   a == NULL && n > 1 → 0
 *   equal adjacent elements → 0 (strict, not nondecreasing)
 */
int
gj_is_sorted_strict_u64_u(const uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (cN <= 1u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}
	return b3531_is_sorted_strict(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_sorted_strict_u64_u(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_is_sorted_strict_u64_u")));
