/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch733: test nondecreasing unsigned char order.
 *
 * Surface (unique symbols):
 *   int gj_mem_is_sorted_u8(const unsigned char *a, size_t n);
 *     — Return 1 (OK) if a[0..n) is sorted nondecreasing
 *       (a[i] <= a[i+1]), else 0 (FAIL). a == NULL with n > 1 → 0;
 *       n <= 1 → 1 (vacuously sorted). Equal adjacent elements allowed.
 *   int __gj_mem_is_sorted_u8  (alias)
 *   __libcgj_batch733_marker = "libcgj-batch733"
 *
 * Distinct from gj_is_sorted_u32 (batch405) and gj_u32_is_sorted_asc
 * (batch665). Unique gj_mem_is_sorted_u8 surface — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch733_marker[] = "libcgj-batch733";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_is_sorted_u8 — 1 (OK) if nondecreasing, else 0 (FAIL).
 *
 * Edge cases:
 *   n <= 1 → 1 (empty / singleton is sorted)
 *   a == NULL && n > 1 → 0
 *   equal adjacent elements allowed (nondecreasing, not strict)
 */
int
gj_mem_is_sorted_u8(const unsigned char *a, size_t n)
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

int __gj_mem_is_sorted_u8(const unsigned char *a, size_t n)
    __attribute__((alias("gj_mem_is_sorted_u8")));
