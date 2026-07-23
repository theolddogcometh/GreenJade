/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch238: Manacher longest palindromic substring
 * length (O(n) time, O(n) stack buffers, n <= 256).
 *
 * Surface (unique symbols):
 *   size_t gj_manacher_len(const char *s);
 *     — Length of the longest palindromic substring of s via Manacher.
 *       NULL → 0, empty → 0. Input longer than 256 → (size_t)-1.
 *       Stack-only transformed string and radius table; no malloc.
 *   __gj_manacher_len  (alias)
 *   __libcgj_batch238_marker = "libcgj-batch238"
 *
 * Distinct from batch215's expand-around-centers gj_longest_palindrome_len.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>

/* Maximum accepted string length (not including trailing NUL). */
#define B238_MAX 256

const char __libcgj_batch238_marker[] = "libcgj-batch238";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b238_strnlen_cap — measure NUL-terminated length without libc.
 * Caps scan at nMax+1 so a missing terminator still terminates.
 * Returns length if 0..nMax, or (size_t)-1 if longer than nMax.
 */
static size_t
b238_strnlen_cap(const char *sz, size_t nMax)
{
	size_t n;

	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return (size_t)-1;
}

/*
 * b238_manacher — Manacher over transformed string t of length m.
 *
 * t has the form # c0 # c1 # ... # c{n-1} # so every palindrome is
 * odd-length in t. p[i] is the radius (in t steps) of the longest
 * palindrome centered at i. The corresponding original-string length
 * equals p[i].
 *
 * Uses the classic center/right mirror acceleration for O(m) total
 * expansions. p[] and t[] are caller-owned stack arrays.
 */
static size_t
b238_manacher(const char *t, int m, int *p)
{
	int i;
	int center;
	int right;
	int best;

	for (i = 0; i < m; i++) {
		p[i] = 0;
	}

	center = 0;
	right = 0;
	best = 0;

	for (i = 0; i < m; i++) {
		if (i < right) {
			int mir = 2 * center - i;
			int lim = right - i;

			p[i] = (p[mir] < lim) ? p[mir] : lim;
		}

		/* Expand around i while within bounds and characters match. */
		while ((i - p[i] - 1) >= 0 && (i + p[i] + 1) < m &&
		       t[i - p[i] - 1] == t[i + p[i] + 1]) {
			p[i]++;
		}

		if (i + p[i] > right) {
			center = i;
			right = i + p[i];
		}
		if (p[i] > best) {
			best = p[i];
		}
	}

	return (size_t)best;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_manacher_len — longest palindromic substring length via Manacher.
 *
 * Builds a separator-expanded view of s on the stack:
 *   t = # s[0] # s[1] # ... # s[n-1] #    (length 2n+1)
 * then runs Manacher to obtain max radius, which equals the original
 * palindrome length.
 *
 * Returns:
 *   0           — s is NULL or empty
 *   (size_t)-1  — strlen(s) > 256 (stack budget exceeded)
 *   1..n        — max palindromic substring length otherwise
 */
size_t
gj_manacher_len(const char *s)
{
	size_t n;
	int m;
	int i;
	char t[2 * B238_MAX + 1];
	int p[2 * B238_MAX + 1];

	if (s == NULL) {
		return 0u;
	}

	n = b238_strnlen_cap(s, (size_t)B238_MAX);
	if (n == (size_t)-1) {
		return (size_t)-1;
	}
	if (n == 0u) {
		return 0u;
	}

	/* t[2*j] = '#', t[2*j+1] = s[j] for j in 0..n-1; final '#'. */
	m = 2 * (int)n + 1;
	for (i = 0; i < m; i++) {
		if ((i & 1) == 0) {
			t[i] = '#';
		} else {
			t[i] = s[i / 2];
		}
	}

	return b238_manacher(t, m, p);
}

size_t __gj_manacher_len(const char *s)
    __attribute__((alias("gj_manacher_len")));
