/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1403: test nonincreasing uint64_t order (desc).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_sorted_desc(const uint64_t *a, size_t n);
 *     — Return 1 (OK) if a[0..n) is sorted nonincreasing (a[i] >= a[i+1]),
 *       else 0 (FAIL). a == NULL with n > 1 → 0; n <= 1 → 1 (vacuously
 *       sorted). Equal adjacent elements allowed (nonincreasing, not
 *       strict).
 *   int __gj_u64_is_sorted_desc  (alias)
 *   __libcgj_batch1403_marker = "libcgj-batch1403"
 *
 * Distinct from gj_u64_is_sorted_asc (batch1402). Unique gj_ names only;
 * do not multi-def prior batch symbols.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1403_marker[] = "libcgj-batch1403";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_sorted_desc — 1 (OK) if nonincreasing, else 0 (FAIL).
 *
 * Edge cases:
 *   n <= 1 → 1 (empty / singleton is sorted)
 *   a == NULL && n > 1 → 0
 *   equal adjacent elements allowed (nonincreasing, not strict)
 */
int
gj_u64_is_sorted_desc(const uint64_t *a, size_t n)
{
	size_t i;

	if (n <= 1u) {
		return 1;
	}
	if (a == NULL) {
		return 0;
	}

	for (i = 1u; i < n; i++) {
		if (a[i] > a[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_sorted_desc(const uint64_t *a, size_t n)
    __attribute__((alias("gj_u64_is_sorted_desc")));
