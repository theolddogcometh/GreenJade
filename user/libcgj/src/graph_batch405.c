/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch405: test nondecreasing uint32_t order.
 *
 * Surface (unique symbols):
 *   int gj_is_sorted_u32(const uint32_t *a, size_t n);
 *     — Return 1 if a[0..n) is sorted nondecreasing (a[i] <= a[i+1]),
 *       else 0. a == NULL with n > 0 → 0; n < 2 → 1 (vacuously sorted).
 *   int __gj_is_sorted_u32  (alias)
 *   __libcgj_batch405_marker = "libcgj-batch405"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch405_marker[] = "libcgj-batch405";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_sorted_u32 — 1 if nondecreasing, else 0.
 *
 * Edge cases:
 *   n < 2 → 1 (empty / singleton is sorted)
 *   a == NULL && n >= 2 → 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 */
int
gj_is_sorted_u32(const uint32_t *a, size_t n)
{
	size_t i;

	if (n < 2u) {
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

int __gj_is_sorted_u32(const uint32_t *a, size_t n)
    __attribute__((alias("gj_is_sorted_u32")));
