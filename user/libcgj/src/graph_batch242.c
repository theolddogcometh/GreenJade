/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch242: naive suffix array (sort suffix indices).
 *
 * Surface (unique symbols):
 *   int gj_suffix_array(const char *s, size_t n, int *sa);
 *     — Fill sa[0..n) with a permutation of 0..n-1 such that
 *       s[sa[0]..n) <= s[sa[1]..n) <= ... in lexicographic order
 *       (unsigned char). n must be <= 128. Returns 0 on success, -1
 *       on bad arguments (NULL s/sa with n>0, or n>128).
 *     — Naive O(n^2 log n) via comparison of full suffixes + insertion
 *       sort (no libc qsort; freestanding-safe).
 *   __gj_suffix_array  (alias)
 *   __libcgj_batch242_marker = "libcgj-batch242"
 *
 * Clean-room freestanding pure C. Compiles with -ffreestanding -msse2.
 * No malloc, no errno, no third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch242_marker[] = "libcgj-batch242";

/* ---- freestanding helpers ---------------------------------------------- */

enum { B242_MAX_N = 128 };

/*
 * b242_cmp_suf — compare suffixes s[i..n) and s[j..n).
 *
 * Returns <0 if s[i..) < s[j..), 0 if equal, >0 if s[i..) > s[j..).
 * Bytes are compared as unsigned char. If one suffix is a proper
 * prefix of the other, the shorter one is smaller.
 */
static int
b242_cmp_suf(const char *s, size_t n, int i, int j)
{
	size_t a = (size_t)i;
	size_t b = (size_t)j;

	while (a < n && b < n) {
		unsigned char ca = (unsigned char)s[a];
		unsigned char cb = (unsigned char)s[b];

		if (ca != cb) {
			return (ca < cb) ? -1 : 1;
		}
		a++;
		b++;
	}
	/* equal so far: shorter remaining length sorts first */
	if (a == b) {
		return 0;
	}
	return (a == n) ? -1 : 1;
}

/*
 * b242_sort_sa — insertion sort of sa[0..n) by suffix order of s.
 * Stable; O(n^2) comparisons, each comparison O(n) → O(n^3) worst,
 * which is fine for n <= 128 (still "naive" class).
 */
static void
b242_sort_sa(const char *s, size_t n, int *sa)
{
	size_t i;
	size_t j;
	int key;

	for (i = 1u; i < n; i++) {
		key = sa[i];
		j = i;
		while (j > 0u && b242_cmp_suf(s, n, key, sa[j - 1u]) < 0) {
			sa[j] = sa[j - 1u];
			j--;
		}
		sa[j] = key;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_suffix_array — naive suffix array: sort indices of all suffixes.
 *
 * On success sa[k] is the starting index of the k-th smallest suffix
 * of s[0..n). sa is a permutation of {0,1,...,n-1}.
 *
 * Returns:
 *   0  — ok (including n == 0: no writes)
 *  -1  — s or sa is NULL with n > 0, or n > 128
 */
int
gj_suffix_array(const char *s, size_t n, int *sa)
{
	size_t i;

	if (n > (size_t)B242_MAX_N) {
		return -1;
	}
	if (n == 0u) {
		return 0;
	}
	if (s == NULL || sa == NULL) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		sa[i] = (int)i;
	}
	b242_sort_sa(s, n, sa);
	return 0;
}

int __gj_suffix_array(const char *s, size_t n, int *sa)
    __attribute__((alias("gj_suffix_array")));
