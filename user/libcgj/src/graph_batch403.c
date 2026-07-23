/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch403: linear search for a uint32_t value.
 *
 * Surface (unique symbols):
 *   size_t gj_find_u32(const uint32_t *a, size_t n, uint32_t v);
 *     — Return the least index i with a[i] == v, or n if not found
 *       (also when a == NULL or n == 0).
 *   size_t __gj_find_u32  (alias)
 *   __libcgj_batch403_marker = "libcgj-batch403"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch403_marker[] = "libcgj-batch403";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_find_u32 — first index of v in a[0..n), or n if absent.
 *
 * Edge cases:
 *   a == NULL → n
 *   n == 0 → 0 (empty range: not found sentinel is n)
 *   multiple matches → lowest index
 */
size_t
gj_find_u32(const uint32_t *a, size_t n, uint32_t v)
{
	size_t i;

	if (a == NULL) {
		return n;
	}

	for (i = 0u; i < n; i++) {
		if (a[i] == v) {
			return i;
		}
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_find_u32(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_find_u32")));
