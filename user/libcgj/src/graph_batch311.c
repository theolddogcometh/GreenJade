/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch311: sorted int32 fixed-array insert/find
 * (binary search / lower-bound).
 *
 * Surface (unique symbols):
 *   int gj_sorted_insert_i32(int32_t *a, size_t *n, size_t cap, int32_t x);
 *     — Keep a[0..*n) sorted ascending in a fixed buffer of capacity cap.
 *       If x is already present: no-op, return 0 (set semantics, no dups).
 *       If absent and *n < cap: lower-bound insert (shift right), bump *n,
 *       return 0.
 *       If full, *n > cap, or NULL a/n: return -1.
 *   int gj_sorted_find_i32(const int32_t *a, size_t n, int32_t x);
 *     — Binary search for x in sorted a[0..n). Returns index in [0, n) on
 *       hit, or -1 on miss / bad args (NULL a with n > 0).
 *   __gj_sorted_insert_i32 / __gj_sorted_find_i32  (aliases)
 *   __libcgj_batch311_marker = "libcgj-batch311"
 *
 * Distinct from batch178 (parallel int key/val map) — single int32 array,
 * find returns index not a mapped value.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch311_marker[] = "libcgj-batch311";

/* ---- lower-bound binary search ----------------------------------------- */

/*
 * Return the smallest index i in [0, n] such that a[i] >= x
 * (or n if all elements are < x). a[0..n) must be sorted ascending.
 */
static size_t
b311_lower_bound(const int32_t *a, size_t n, int32_t x)
{
	size_t lo = 0u;
	size_t hi = n;

	while (lo < hi) {
		size_t mid = lo + ((hi - lo) >> 1);

		if (a[mid] < x) {
			lo = mid + 1u;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/* ---- gj_sorted_find_i32 ------------------------------------------------ */

/*
 * gj_sorted_find_i32 — binary-search for x in sorted a[0..n).
 *
 * a: sorted ascending array of n int32_t (NULL only valid when n == 0)
 * n: length
 * x: value to locate
 *
 * Returns index i with a[i] == x, or -1 if not found / invalid args.
 * Does not set errno (freestanding).
 */
int
gj_sorted_find_i32(const int32_t *a, size_t n, int32_t x)
{
	size_t i;

	if (n == 0u) {
		return -1;
	}
	if (a == NULL) {
		return -1;
	}

	i = b311_lower_bound(a, n, x);
	if (i < n && a[i] == x) {
		/* Practical fixed arrays fit int; index is in [0, n). */
		return (int)i;
	}
	return -1;
}

int __gj_sorted_find_i32(const int32_t *a, size_t n, int32_t x)
    __attribute__((alias("gj_sorted_find_i32")));

/* ---- gj_sorted_insert_i32 ---------------------------------------------- */

/*
 * gj_sorted_insert_i32 — insert x into sorted fixed array a[0..*n).
 *
 * a:   mutable sorted array storage (capacity cap)
 * n:   in/out length; *n elements are valid and sorted before call
 * cap: maximum number of elements a can hold
 * x:   value to insert
 *
 * Set semantics: existing x is a successful no-op (return 0).
 * New x with room: insert at lower-bound, shift [i..*n) right, *n += 1.
 * Returns 0 on success, -1 on NULL a/n, *n > cap, or full (*n >= cap).
 */
int
gj_sorted_insert_i32(int32_t *a, size_t *n, size_t cap, int32_t x)
{
	size_t i;
	size_t cur;
	size_t j;

	if (a == NULL || n == NULL) {
		return -1;
	}
	cur = *n;
	if (cur > cap) {
		return -1;
	}

	i = b311_lower_bound(a, cur, x);
	if (i < cur && a[i] == x) {
		/* Already present — keep unique, sorted order intact. */
		return 0;
	}
	if (cur >= cap) {
		return -1;
	}

	/* Shift [i .. cur) one slot right to open the insertion point. */
	for (j = cur; j > i; j--) {
		a[j] = a[j - 1u];
	}
	a[i] = x;
	*n = cur + 1u;
	return 0;
}

int __gj_sorted_insert_i32(int32_t *a, size_t *n, size_t cap, int32_t x)
    __attribute__((alias("gj_sorted_insert_i32")));
