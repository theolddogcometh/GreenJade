/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch612: freestanding count of matching
 * uint32_t values in a bounded array.
 *
 * Surface (unique symbols; distinct from gj_count_eq_u32 /
 * __gj_count_eq_u32 in graph_batch440.c — no multi-def):
 *   size_t gj_u32_count(const uint32_t *a, size_t n, uint32_t v);
 *     — Return how many elements of a[0..n) equal v. NULL a with n > 0
 *       yields 0; n == 0 yields 0.
 *   size_t __gj_u32_count  (alias)
 *   __libcgj_batch612_marker = "libcgj-batch612"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch612_marker[] = "libcgj-batch612";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_count — count elements equal to v.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 * v: value to match
 *
 * Returns the number of matches in a[0..n).
 */
size_t
gj_u32_count(const uint32_t *a, size_t n, uint32_t v)
{
	size_t i;
	size_t c;

	if (n == 0u || a == NULL) {
		return 0u;
	}

	c = 0u;
	for (i = 0u; i < n; i++) {
		if (a[i] == v) {
			c++;
		}
	}
	return c;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_count(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_count")));
