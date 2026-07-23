/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1563: bounded LCS length (n <= 32).
 *
 * Surface (unique symbols):
 *   int gj_str_lcs_len_bound(const char *a, const char *b);
 *     — Longest common subsequence length via stack DP. Both strings
 *       capped at B1563_MAX_LEN (32) excluding NUL. Returns LCS length
 *       in 0..32 on success; -1 on NULL or oversize.
 *       Distinct from gj_lcs_len (batch214, max 64).
 *   int __gj_str_lcs_len_bound  (alias)
 *   __libcgj_batch1563_marker = "libcgj-batch1563"
 *
 * DP: two rolling rows of length nLenB+1 on the stack (max 33 ints).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

#define B1563_MAX_LEN 32

const char __libcgj_batch1563_marker[] = "libcgj-batch1563";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1563_strnlen_cap(const char *sz, int nMax)
{
	int n;

	for (n = 0; n <= nMax; n++) {
		if (sz[n] == '\0') {
			return n;
		}
	}
	return -1;
}

static int
b1563_max2(int nA, int nB)
{
	return (nA > nB) ? nA : nB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_lcs_len_bound — stack-DP LCS length, max length 32.
 */
int
gj_str_lcs_len_bound(const char *a, const char *b)
{
	int nLenA;
	int nLenB;
	int i;
	int j;
	int aPrev[B1563_MAX_LEN + 1];
	int aCur[B1563_MAX_LEN + 1];

	if (a == NULL || b == NULL) {
		return -1;
	}

	nLenA = b1563_strnlen_cap(a, B1563_MAX_LEN);
	nLenB = b1563_strnlen_cap(b, B1563_MAX_LEN);
	if (nLenA < 0 || nLenB < 0) {
		return -1;
	}

	if (nLenA == 0 || nLenB == 0) {
		return 0;
	}

	for (j = 0; j <= nLenB; j++) {
		aPrev[j] = 0;
	}

	for (i = 1; i <= nLenA; i++) {
		unsigned char chA = (unsigned char)a[i - 1];

		aCur[0] = 0;
		for (j = 1; j <= nLenB; j++) {
			unsigned char chB = (unsigned char)b[j - 1];

			if (chA == chB) {
				aCur[j] = aPrev[j - 1] + 1;
			} else {
				aCur[j] = b1563_max2(aPrev[j], aCur[j - 1]);
			}
		}
		for (j = 0; j <= nLenB; j++) {
			aPrev[j] = aCur[j];
		}
	}

	return aPrev[nLenB];
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_lcs_len_bound(const char *a, const char *b)
    __attribute__((alias("gj_str_lcs_len_bound")));
