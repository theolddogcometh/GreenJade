/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch313: coordinate compression of int64 keys.
 *
 * Surface (unique symbols):
 *   size_t gj_coord_compress(int64_t *a, size_t n, int64_t *uniq_out,
 *                            size_t uniq_cap);
 *     — Coordinate-compress a[0..n) to dense ranks 0 .. uniq-1.
 *       Builds a sorted unique table of the original values, optionally
 *       copies that table into uniq_out[0..min(uniq,uniq_cap)), then
 *       rewrites a[i] in place as the rank of the original a[i]
 *       (as int64_t). Ranks are stable by value order (smallest → 0).
 *       Returns the number of unique values (uniq). Constraints:
 *         n <= 256 (stack working set); n > 256 or a == NULL with n > 0
 *         → return 0 and leave a untouched.
 *         n == 0 → return 0.
 *         uniq_out may be NULL or uniq_cap == 0 (ranks still written).
 *         uniq_out must not alias a when both are written.
 *   __gj_coord_compress  (alias)
 *   __libcgj_batch313_marker = "libcgj-batch313"
 *
 * Algorithm (freestanding, no malloc):
 *   1. Copy a[0..n) into a stack buffer of capacity 256.
 *   2. Insertion-sort the copy ascending.
 *   3. Collapse adjacent duplicates (unique).
 *   4. Optionally emit the unique values into uniq_out.
 *   5. Binary-search each original a[i] in the unique table; store rank.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from batch210 (radix u32) and
 * batch211 (median select).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch313_marker[] = "libcgj-batch313";

/* Max keys for stack working set (n <= 256). */
#define B313_MAX 256u

/* ---- helpers ----------------------------------------------------------- */

/*
 * b313_insert_sort — ascending insertion sort of pA[0..cN).
 * Stable for equal keys; only used for n <= B313_MAX.
 */
static void
b313_insert_sort(int64_t *pA, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		int64_t i64Key = pA[i];
		size_t j = i;

		while (j > 0u && pA[j - 1u] > i64Key) {
			pA[j] = pA[j - 1u];
			j--;
		}
		pA[j] = i64Key;
	}
}

/*
 * b313_unique — in-place collapse of adjacent duplicates on a sorted
 * array pA[0..cN). Returns the new length (number of unique values).
 * When cN == 0 returns 0 without touching pA.
 */
static size_t
b313_unique(int64_t *pA, size_t cN)
{
	size_t i;
	size_t cOut;

	if (cN == 0u) {
		return 0u;
	}

	cOut = 1u;
	for (i = 1u; i < cN; i++) {
		if (pA[i] != pA[cOut - 1u]) {
			pA[cOut] = pA[i];
			cOut++;
		}
	}
	return cOut;
}

/*
 * b313_rank — lower-bound binary search: index of i64Key in sorted unique
 * table pUniq[0..cUniq). Caller guarantees i64Key is present, so the
 * search always hits an equal element. Returns rank in [0, cUniq).
 */
static size_t
b313_rank(const int64_t *pUniq, size_t cUniq, int64_t i64Key)
{
	size_t lo = 0u;
	size_t hi = cUniq;

	while (lo < hi) {
		size_t mid = lo + ((hi - lo) >> 1);

		if (pUniq[mid] < i64Key) {
			lo = mid + 1u;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_coord_compress — map a[0..n) to ranks 0..uniq-1 in place.
 *
 * a:         mutable array of n int64_t keys (rewritten as ranks on success)
 * n:         length; must be in [0, 256]
 * uniq_out:  optional destination for sorted unique original values
 * uniq_cap:  capacity of uniq_out (elements); may be 0
 *
 * Returns the unique count. On failure (NULL a with n > 0, or n > 256)
 * returns 0 and does not modify a. When uniq_out is non-NULL, writes
 * min(uniq, uniq_cap) sorted unique values (the first that many ranks'
 * originals). Freestanding: never sets errno.
 */
size_t
gj_coord_compress(int64_t *a, size_t n, int64_t *uniq_out, size_t uniq_cap)
{
	int64_t aTmp[B313_MAX];
	size_t cUniq;
	size_t i;
	size_t cEmit;

	if (n == 0u) {
		return 0u;
	}
	if (a == NULL || n > B313_MAX) {
		return 0u;
	}

	/* 1. Stack copy of the original keys (a will be rewritten as ranks). */
	for (i = 0u; i < n; i++) {
		aTmp[i] = a[i];
	}

	/* 2. Sort the working copy ascending. */
	b313_insert_sort(aTmp, n);

	/* 3. Unique collapse → dense value table aTmp[0..cUniq). */
	cUniq = b313_unique(aTmp, n);

	/* 4. Optional emit of the unique table. */
	if (uniq_out != NULL && uniq_cap > 0u && cUniq > 0u) {
		cEmit = cUniq;
		if (cEmit > uniq_cap) {
			cEmit = uniq_cap;
		}
		for (i = 0u; i < cEmit; i++) {
			uniq_out[i] = aTmp[i];
		}
	}

	/* 5. Map each original a[i] to its rank in the unique table. */
	for (i = 0u; i < n; i++) {
		a[i] = (int64_t)b313_rank(aTmp, cUniq, a[i]);
	}

	return cUniq;
}

size_t __gj_coord_compress(int64_t *a, size_t n, int64_t *uniq_out,
    size_t uniq_cap) __attribute__((alias("gj_coord_compress")));
