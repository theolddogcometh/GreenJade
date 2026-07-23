/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch312: merge overlapping int64 half-open intervals.
 *
 * Surface (unique symbols):
 *   size_t gj_interval_merge(int64_t *starts, int64_t *ends, size_t n);
 *     — Sort intervals [starts[i], ends[i]) by ascending start, then merge
 *       overlapping and abutting ranges in place. On return, the first
 *       return-value elements of starts/ends are the merged, sorted,
 *       non-overlapping cover. Returns the new interval count.
 *       Empty intervals (start >= end) are dropped. NULL starts or ends
 *       with n > 0 returns 0 (freestanding, no errno). n == 0 returns 0.
 *   __gj_interval_merge  (alias)
 *   __libcgj_batch312_marker = "libcgj-batch312"
 *
 * Sort strategy (no malloc):
 *   - n <= 128: copy onto stack buffers and insertion-sort there, then
 *     write back (bounded stack: 128 * 2 * 8 bytes for values).
 *   - n > 128: in-place pair heapsort (O(1) extra, same order predicate).
 * Merge is a single linear pass after sort.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch312_marker[] = "libcgj-batch312";

/* Max intervals sorted via stack temp (128 * sizeof(int64_t) * 2). */
#define B312_STACK_MAX 128u

/* ---- helpers ----------------------------------------------------------- */

static void
b312_swap_i64(int64_t *pA, int64_t *pB)
{
	int64_t i64T = *pA;

	*pA = *pB;
	*pB = i64T;
}

/* Swap interval pair i with j in starts/ends. */
static void
b312_swap_pair(int64_t *pStarts, int64_t *pEnds, size_t i, size_t j)
{
	b312_swap_i64(&pStarts[i], &pStarts[j]);
	b312_swap_i64(&pEnds[i], &pEnds[j]);
}

/*
 * Order: ascending start; equal start → ascending end (stable-ish for
 * insertion, deterministic for heap).
 */
static int
b312_less(int64_t i64Sa, int64_t i64Ea, int64_t i64Sb, int64_t i64Eb)
{
	if (i64Sa < i64Sb) {
		return 1;
	}
	if (i64Sa > i64Sb) {
		return 0;
	}
	return (i64Ea < i64Eb) ? 1 : 0;
}

/* Insertion sort of interval pairs in starts/ends[0..cN). */
static void
b312_insertion_sort(int64_t *pStarts, int64_t *pEnds, size_t cN)
{
	size_t i;

	for (i = 1u; i < cN; i++) {
		int64_t i64S = pStarts[i];
		int64_t i64E = pEnds[i];
		size_t j = i;

		while (j > 0u &&
		    b312_less(i64S, i64E, pStarts[j - 1u], pEnds[j - 1u])) {
			pStarts[j] = pStarts[j - 1u];
			pEnds[j] = pEnds[j - 1u];
			j--;
		}
		pStarts[j] = i64S;
		pEnds[j] = i64E;
	}
}

/* Heap sift-down for max-heap ordered by b312_less (parent "greater"). */
static void
b312_sift_down(int64_t *pStarts, int64_t *pEnds, size_t cN, size_t iRoot)
{
	size_t i = iRoot;

	for (;;) {
		size_t iLeft = i * 2u + 1u;
		size_t iRight = iLeft + 1u;
		size_t iLargest = i;

		if (iLeft < cN &&
		    b312_less(pStarts[iLargest], pEnds[iLargest],
		        pStarts[iLeft], pEnds[iLeft])) {
			iLargest = iLeft;
		}
		if (iRight < cN &&
		    b312_less(pStarts[iLargest], pEnds[iLargest],
		        pStarts[iRight], pEnds[iRight])) {
			iLargest = iRight;
		}
		if (iLargest == i) {
			return;
		}
		b312_swap_pair(pStarts, pEnds, i, iLargest);
		i = iLargest;
	}
}

/* In-place heapsort of interval pairs (O(1) aux). */
static void
b312_heapsort(int64_t *pStarts, int64_t *pEnds, size_t cN)
{
	size_t i;

	if (cN < 2u) {
		return;
	}

	/* Build max-heap. */
	i = cN / 2u;
	while (i > 0u) {
		i--;
		b312_sift_down(pStarts, pEnds, cN, i);
	}

	/* Extract max repeatedly. */
	for (i = cN; i > 1u; i--) {
		b312_swap_pair(pStarts, pEnds, 0u, i - 1u);
		b312_sift_down(pStarts, pEnds, i - 1u, 0u);
	}
}

/*
 * Sort interval pairs by start (then end).
 * n <= B312_STACK_MAX: copy to stack, insertion-sort, copy back.
 * n > B312_STACK_MAX: in-place heapsort.
 */
static void
b312_sort(int64_t *pStarts, int64_t *pEnds, size_t cN)
{
	if (cN < 2u) {
		return;
	}

	if (cN <= (size_t)B312_STACK_MAX) {
		int64_t aStarts[B312_STACK_MAX];
		int64_t aEnds[B312_STACK_MAX];
		size_t i;

		for (i = 0u; i < cN; i++) {
			aStarts[i] = pStarts[i];
			aEnds[i] = pEnds[i];
		}
		b312_insertion_sort(aStarts, aEnds, cN);
		for (i = 0u; i < cN; i++) {
			pStarts[i] = aStarts[i];
			pEnds[i] = aEnds[i];
		}
		return;
	}

	b312_heapsort(pStarts, pEnds, cN);
}

/*
 * Linear merge of sorted half-open intervals in place.
 * Drops empty (start >= end). Merges when next.start <= cur.end
 * (overlap or abut). Returns new count.
 */
static size_t
b312_merge_sorted(int64_t *pStarts, int64_t *pEnds, size_t cN)
{
	size_t i;
	size_t cOut = 0u;

	for (i = 0u; i < cN; i++) {
		int64_t i64S = pStarts[i];
		int64_t i64E = pEnds[i];

		/* Empty interval contributes no coverage. */
		if (i64S >= i64E) {
			continue;
		}

		if (cOut == 0u) {
			pStarts[0] = i64S;
			pEnds[0] = i64E;
			cOut = 1u;
			continue;
		}

		/* Overlap or abut: extend the open end if needed. */
		if (i64S <= pEnds[cOut - 1u]) {
			if (i64E > pEnds[cOut - 1u]) {
				pEnds[cOut - 1u] = i64E;
			}
		} else {
			pStarts[cOut] = i64S;
			pEnds[cOut] = i64E;
			cOut++;
		}
	}

	return cOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_interval_merge — sort + merge half-open [start, end) int64 intervals.
 *
 * starts, ends: parallel arrays of length n (mutated in place).
 * n:            input interval count.
 *
 * After success, starts[0..ret) / ends[0..ret) are sorted, non-empty,
 * non-overlapping, and non-abutting (adjacent ranges are fused). Elements
 * at indices >= ret are unspecified.
 *
 * Returns:
 *   new count of merged intervals (0 .. n)
 *   0 if n == 0, or starts/ends is NULL with n > 0
 */
size_t
gj_interval_merge(int64_t *starts, int64_t *ends, size_t n)
{
	if (n == 0u) {
		return 0u;
	}
	if (starts == NULL || ends == NULL) {
		return 0u;
	}

	b312_sort(starts, ends, n);
	return b312_merge_sorted(starts, ends, n);
}

size_t __gj_interval_merge(int64_t *starts, int64_t *ends, size_t n)
    __attribute__((alias("gj_interval_merge")));
