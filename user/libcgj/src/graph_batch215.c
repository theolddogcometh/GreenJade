/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch215: longest palindromic substring length
 * via expand-around-centers (O(n^2) time, O(1) extra space).
 *
 * Surface (unique symbols):
 *   int gj_longest_palindrome_len(const char *s);
 *     — Length of the longest palindromic substring of s (byte string).
 *       Empty / NULL → 0. Compact expand-around-centers: every index is
 *       a center for odd-length and even-length candidates.
 *   __gj_longest_palindrome_len  (alias)
 *   __libcgj_batch215_marker = "libcgj-batch215"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch215_marker[] = "libcgj-batch215";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b215_strlen(const char *s)
{
	size_t n = 0u;

	if (s == NULL) {
		return 0u;
	}
	while (s[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Expand while s[l..r] is a palindrome; return its length.
 * l/r are signed so the left edge can walk past 0 safely.
 */
static int
b215_expand(const char *s, int n, int l, int r)
{
	while (l >= 0 && r < n && s[l] == s[r]) {
		l--;
		r++;
	}
	/* After overshoot: length = (r-1) - (l+1) + 1 = r - l - 1 */
	return r - l - 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_longest_palindrome_len — longest palindromic substring length.
 *
 * For each index i in [0, n):
 *   odd  center at i      → expand(i, i)
 *   even center at i,i+1  → expand(i, i+1)
 * Track the maximum length. O(n^2) comparisons, O(1) extra memory.
 *
 * Returns 0 for NULL or empty s; otherwise the max palindrome length
 * in [1, n] (single characters are length-1 palindromes).
 */
int
gj_longest_palindrome_len(const char *s)
{
	int n;
	int i;
	int best;
	int len;

	if (s == NULL) {
		return 0;
	}

	n = (int)b215_strlen(s);
	if (n <= 1) {
		return n;
	}

	best = 1;
	for (i = 0; i < n; i++) {
		len = b215_expand(s, n, i, i);
		if (len > best) {
			best = len;
		}
		len = b215_expand(s, n, i, i + 1);
		if (len > best) {
			best = len;
		}
	}
	return best;
}

int __gj_longest_palindrome_len(const char *s)
    __attribute__((alias("gj_longest_palindrome_len")));
