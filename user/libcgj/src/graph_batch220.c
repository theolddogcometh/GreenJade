/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch220: Fenwick tree (binary indexed tree) over
 * int64_t values — point update and prefix sum.
 *
 * Surface (unique symbols):
 *   void gj_fenwick_add(int64_t *bit, size_t n, size_t i, int64_t delta);
 *     — add delta to logical index i (0-based). No-op if bit is NULL,
 *       n is 0, or i >= n. bit[] is the tree array of length n (caller
 *       zero-initializes before first use).
 *   int64_t gj_fenwick_sum(const int64_t *bit, size_t n, size_t i);
 *     — prefix sum over logical indices 0..i inclusive. Returns 0 if
 *       bit is NULL, n is 0, or i is out of range (i >= n is treated
 *       as empty / invalid → 0).
 *   __gj_fenwick_add / __gj_fenwick_sum  (aliases)
 *   __libcgj_batch220_marker = "libcgj-batch220"
 *
 * Layout: 0-based logical indices mapped to classic 1-based Fenwick
 * storage in the same bit[0..n-1] buffer (tree index j uses bit[j-1]).
 * Update walks j += j & -j; query walks j -= j & -j. size_t two's
 * complement gives the least-set-bit step.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch220_marker[] = "libcgj-batch220";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fenwick_add — point update: A[i] += delta in O(log n).
 *
 * bit: tree storage of length n (modified in place).
 * n:   number of logical positions (and tree slots).
 * i:   0-based index to update; must be < n.
 * delta: signed amount to add (may be negative).
 */
void
gj_fenwick_add(int64_t *bit, size_t n, size_t i, int64_t delta)
{
	size_t j;

	if (bit == NULL || n == 0u || i >= n) {
		return;
	}

	/* Map 0-based logical index to 1-based Fenwick index. */
	j = i + 1u;
	while (j <= n) {
		bit[j - 1u] += delta;
		j += j & (size_t)(0u - j); /* j & -j least set bit */
	}
}

/*
 * gj_fenwick_sum — prefix sum A[0] + ... + A[i] in O(log n).
 *
 * bit: tree storage of length n (read-only).
 * n:   number of logical positions.
 * i:   inclusive end of the prefix (0-based). i >= n → 0.
 */
int64_t
gj_fenwick_sum(const int64_t *bit, size_t n, size_t i)
{
	size_t j;
	int64_t s;

	if (bit == NULL || n == 0u || i >= n) {
		return (int64_t)0;
	}

	j = i + 1u;
	s = (int64_t)0;
	while (j > 0u) {
		s += bit[j - 1u];
		j -= j & (size_t)(0u - j); /* j & -j least set bit */
	}
	return s;
}

void __gj_fenwick_add(int64_t *bit, size_t n, size_t i, int64_t delta)
    __attribute__((alias("gj_fenwick_add")));

int64_t __gj_fenwick_sum(const int64_t *bit, size_t n, size_t i)
    __attribute__((alias("gj_fenwick_sum")));
