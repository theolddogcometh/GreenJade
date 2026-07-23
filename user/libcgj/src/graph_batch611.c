/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch611: linear search for first matching uint32_t.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_find(const uint32_t *a, size_t n, uint32_t v);
 *     — Return the lowest index i in [0, n) such that a[i] == v.
 *       If not found, return n. n == 0 returns 0. NULL a with n > 0
 *       yields n (not found).
 *   size_t __gj_u32_find  (alias)
 *   __libcgj_batch611_marker = "libcgj-batch611"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch611_marker[] = "libcgj-batch611";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_find — first index of value v in a[0..n), or n if absent.
 *
 * a: base array (NULL with n > 0 → return n)
 * n: element count
 * v: value to locate
 *
 * Linear scan; returns the first match index, or n when not found.
 */
size_t
gj_u32_find(const uint32_t *a, size_t n, uint32_t v)
{
	size_t i;

	if (n == 0u || a == NULL) {
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

size_t __gj_u32_find(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_find")));
