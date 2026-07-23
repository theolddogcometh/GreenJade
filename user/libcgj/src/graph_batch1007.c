/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1007: insert one uint32_t at an index in a
 * capacity-bounded array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_insert_at(uint32_t *a, size_t n, size_t cap, size_t i,
 *                           uint32_t v);
 *     — Insert v at index i of a[0..n), shifting a[i..n) one slot right.
 *       Requires room for one more element (n < cap) and i <= n.
 *       Returns the new length (n+1) on success, or the old n on failure
 *       (NULL a, n >= cap, or i > n). On failure the array is unchanged.
 *   size_t __gj_u32_insert_at  (alias)
 *   __libcgj_batch1007_marker = "libcgj-batch1007"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1007_marker[] = "libcgj-batch1007";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_insert_at — insert v at index i; return new length or old n.
 *
 * a:   array base (NULL → return n)
 * n:   current element count
 * cap: allocated capacity in elements (must be > n to succeed)
 * i:   insertion index in 0..n inclusive (i == n appends)
 * v:   value to insert
 *
 * Shifts a[i..n) right by one, writes a[i] = v, returns n+1. Failures
 * leave a[] unchanged and return n.
 */
size_t
gj_u32_insert_at(uint32_t *a, size_t n, size_t cap, size_t i, uint32_t v)
{
	size_t j;

	if (a == NULL || n >= cap || i > n) {
		return n;
	}

	/* Shift a[i..n) one slot to the right (walk high → low). */
	for (j = n; j > i; j--) {
		a[j] = a[j - 1u];
	}
	a[i] = v;
	return n + 1u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_insert_at(uint32_t *a, size_t n, size_t cap, size_t i,
    uint32_t v)
    __attribute__((alias("gj_u32_insert_at")));
