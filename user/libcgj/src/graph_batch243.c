/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch243: next lexicographic permutation of uint32_t[].
 *
 * Surface (unique symbols):
 *   int gj_next_perm_u32(uint32_t *a, size_t n);
 *     — std::next_permutation-style advance of a[0..n) in lexicographic
 *       order under ascending comparison of uint32_t values.
 *       Returns 1 if a next permutation was produced.
 *       Returns 0 if a was the last permutation (then reversed in place
 *       to the first / sorted-ascending order).
 *       Returns -1 if a is NULL and n > 0.
 *       For n == 0 or n == 1: no change, returns 0 (already last).
 *   __gj_next_perm_u32  (alias)
 *   __libcgj_batch243_marker = "libcgj-batch243"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch243_marker[] = "libcgj-batch243";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap two uint32_t elements. */
static void
b243_swap_u32(uint32_t *x, uint32_t *y)
{
	uint32_t t = *x;

	*x = *y;
	*y = t;
}

/* Reverse a[lo..hi] inclusive. */
static void
b243_reverse_u32(uint32_t *a, size_t lo, size_t hi)
{
	while (lo < hi) {
		b243_swap_u32(&a[lo], &a[hi]);
		lo++;
		hi--;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_next_perm_u32 — next lexicographic permutation of uint32_t array.
 *
 * Classic algorithm (same shape as C++ std::next_permutation):
 *   1. Find rightmost ascent: largest k with a[k] < a[k+1].
 *      If none, reverse whole array → first perm, return 0.
 *   2. Find rightmost l > k with a[k] < a[l]; swap a[k], a[l].
 *   3. Reverse the suffix a[k+1 .. n-1].
 *   4. Return 1.
 *
 * NULL a with n > 0 → -1. Empty / singleton → 0 (already terminal).
 */
int
gj_next_perm_u32(uint32_t *a, size_t n)
{
	size_t k;
	size_t l;

	if (a == NULL) {
		return (n > 0u) ? -1 : 0;
	}
	if (n < 2u) {
		return 0;
	}

	/* 1. rightmost k with a[k] < a[k+1] */
	k = n - 2u;
	for (;;) {
		if (a[k] < a[k + 1u]) {
			break;
		}
		if (k == 0u) {
			/* fully non-ascending: last perm → reverse to first */
			b243_reverse_u32(a, 0u, n - 1u);
			return 0;
		}
		k--;
	}

	/* 2. rightmost l > k with a[k] < a[l] */
	l = n - 1u;
	while (a[k] >= a[l]) {
		l--;
	}
	b243_swap_u32(&a[k], &a[l]);

	/* 3. reverse suffix after pivot */
	b243_reverse_u32(a, k + 1u, n - 1u);
	return 1;
}

int __gj_next_perm_u32(uint32_t *a, size_t n)
    __attribute__((alias("gj_next_perm_u32")));
