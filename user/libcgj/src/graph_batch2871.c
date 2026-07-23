/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2871: linear search for first matching uint64_t
 * (_u arr surface).
 *
 * Surface (unique symbols):
 *   size_t gj_arr_u64_find_u(const uint64_t *a, size_t n, uint64_t v);
 *     - Return the lowest index i in [0, n) such that a[i] == v.
 *       If not found, return n. n == 0 returns 0. NULL a with n > 0
 *       yields n (not found).
 *   size_t __gj_arr_u64_find_u  (alias)
 *   __libcgj_batch2871_marker = "libcgj-batch2871"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_u32_find (batch611)
 * — unique u64 arr _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2871_marker[] = "libcgj-batch2871";

/* ---- freestanding helpers ---------------------------------------------- */

/* First index of v in p[0..n), or n. p non-NULL when n > 0. */
static size_t
b2871_find(const uint64_t *p, size_t n, uint64_t v)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (p[i] == v) {
			return i;
		}
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_find_u - first index of value v in a[0..n), or n if absent.
 *
 * a: base array (NULL with n > 0 -> return n)
 * n: element count
 * v: value to locate
 *
 * Linear scan; returns the first match index, or n when not found.
 */
size_t
gj_arr_u64_find_u(const uint64_t *a, size_t n, uint64_t v)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return n;
	}
	return b2871_find(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arr_u64_find_u(const uint64_t *a, size_t n, uint64_t v)
    __attribute__((alias("gj_arr_u64_find_u")));
