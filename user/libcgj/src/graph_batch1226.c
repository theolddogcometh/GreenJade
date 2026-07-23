/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1226: naive suffix array for small n only.
 *
 * Surface (unique symbols):
 *   int gj_suffix_array_naive(const char *s, size_t n, int *sa);
 *     — Fill sa[0..n) with a permutation of 0..n-1 sorted by suffix
 *       order of s[0..n) (unsigned char, shorter remaining first on
 *       tie-through-end). n must be <= 64. Returns 0 on success; -1
 *       on bad args (NULL s/sa with n>0, or n>64). n==0 → 0.
 *   int __gj_suffix_array_naive  (alias)
 *   __libcgj_batch1226_marker = "libcgj-batch1226"
 *
 * Distinct from gj_suffix_array (batch242, n<=128) — unique name and
 * tighter n bound; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1226_MAX_N = 64 };

const char __libcgj_batch1226_marker[] = "libcgj-batch1226";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1226_cmp_suf(const char *szS, size_t cN, int iA, int iB)
{
	size_t a = (size_t)iA;
	size_t b = (size_t)iB;

	while (a < cN && b < cN) {
		unsigned char uA = (unsigned char)szS[a];
		unsigned char uB = (unsigned char)szS[b];

		if (uA != uB) {
			return (uA < uB) ? -1 : 1;
		}
		a++;
		b++;
	}
	if (a == b) {
		return 0;
	}
	return (a == cN) ? -1 : 1;
}

static void
b1226_sort_sa(const char *szS, size_t cN, int *pSa)
{
	size_t i;
	size_t j;
	int nKey;

	for (i = 1u; i < cN; i++) {
		nKey = pSa[i];
		j = i;
		while (j > 0u && b1226_cmp_suf(szS, cN, nKey, pSa[j - 1u]) < 0) {
			pSa[j] = pSa[j - 1u];
			j--;
		}
		pSa[j] = nKey;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_suffix_array_naive — small-n naive suffix index sort.
 */
int
gj_suffix_array_naive(const char *szS, size_t cN, int *pSa)
{
	size_t i;

	if (cN > (size_t)B1226_MAX_N) {
		return -1;
	}
	if (cN == 0u) {
		return 0;
	}
	if (szS == NULL || pSa == NULL) {
		return -1;
	}

	for (i = 0u; i < cN; i++) {
		pSa[i] = (int)i;
	}
	b1226_sort_sa(szS, cN, pSa);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_suffix_array_naive(const char *szS, size_t cN, int *pSa)
    __attribute__((alias("gj_suffix_array_naive")));
