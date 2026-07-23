/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch665: test nondecreasing uint32_t order (asc).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_sorted_asc(const uint32_t *a, size_t n);
 *     — Return 1 (OK) if a[0..n) is sorted nondecreasing (a[i] <= a[i+1]),
 *       else 0 (FAIL). a == NULL with n > 1 → 0; n <= 1 → 1 (vacuously
 *       sorted). Equal adjacent elements allowed (nondecreasing, not
 *       strict).
 *   int __gj_u32_is_sorted_asc  (alias)
 *   __libcgj_batch665_marker = "libcgj-batch665"
 *
 * Distinct from gj_is_sorted_u32 / __gj_is_sorted_u32 (batch405). Unique
 * gj_ names only; do not multi-def is_sorted symbols from that batch.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch665_marker[] = "libcgj-batch665";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_sorted_asc — 1 (OK) if nondecreasing, else 0 (FAIL).
 *
 * Edge cases:
 *   n <= 1 → 1 (empty / singleton is sorted)
 *   a == NULL && n > 1 → 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 */
int
gj_u32_is_sorted_asc(const uint32_t *a, size_t n)
{
	size_t i;

	if (n <= 1u) {
		return 1;
	}
	if (a == NULL) {
		return 0;
	}

	for (i = 1u; i < n; i++) {
		if (a[i] < a[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_sorted_asc(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_is_sorted_asc")));
