/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1567: common suffix length of two C strings.
 *
 * NOTE: gj_str_common_suffix_len already exists in graph_batch1044.c.
 * This TU exports a distinct n-bounded surface to stay unique at link:
 *
 * Surface (unique symbols):
 *   size_t gj_str_common_suffix_n(const char *a, const char *b, size_t n);
 *     — Shared suffix length of a and b, capped at n octets. Comparison
 *       is unsigned-char ordinal. NULL a/b → 0.
 *   size_t __gj_str_common_suffix_n  (alias)
 *   __libcgj_batch1567_marker = "libcgj-batch1567"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1567_marker[] = "libcgj-batch1567";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1567_len(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_common_suffix_n — shared suffix length, at most n.
 *
 * Theme label was gj_str_common_suffix_len (taken by batch1044); this is
 * the unique n-bounded counterpart for string-metrics batch1567.
 */
size_t
gj_str_common_suffix_n(const char *a, const char *b, size_t n)
{
	size_t nA;
	size_t nB;
	size_t i;
	size_t nCap;

	if (a == NULL || b == NULL || n == 0u) {
		return 0u;
	}

	nA = b1567_len(a);
	nB = b1567_len(b);
	nCap = n;
	if (nCap > nA) {
		nCap = nA;
	}
	if (nCap > nB) {
		nCap = nB;
	}

	i = 0u;
	while (i < nCap &&
	    (unsigned char)a[nA - 1u - i] ==
	        (unsigned char)b[nB - 1u - i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_common_suffix_n(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_common_suffix_n")));
