/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1222: KMP first-match search over explicit lengths.
 *
 * Surface (unique symbols):
 *   long gj_kmp_search_first(const char *hay, size_t nh,
 *                            const char *pat, size_t np);
 *     — First byte index of pat[0..np) in hay[0..nh), or -1 if not
 *       found / bad args. Empty pattern (np==0) → 0 when hay/pat
 *       pointers are acceptable (NULL hay ok only if nh==0). Pattern
 *       longer than 4096 → -1. Distinct from NUL-terminated
 *       gj_kmp_find (batch216).
 *   long __gj_kmp_search_first  (alias)
 *   __libcgj_batch1222_marker = "libcgj-batch1222"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1222_MAX_PAT = 4096 };

const char __libcgj_batch1222_marker[] = "libcgj-batch1222";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1222_build_lps(const char *szPat, size_t cN, int *pLps)
{
	size_t i;
	size_t cLen;

	if (cN == 0u) {
		return 0;
	}
	pLps[0] = 0;
	cLen = 0u;
	i = 1u;
	while (i < cN) {
		if (szPat[i] == szPat[cLen]) {
			cLen++;
			pLps[i] = (int)cLen;
			i++;
		} else if (cLen != 0u) {
			cLen = (size_t)pLps[cLen - 1u];
		} else {
			pLps[i] = 0;
			i++;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kmp_search_first — first index of pat in hay via KMP, or -1.
 */
long
gj_kmp_search_first(const char *szHay, size_t cHay,
    const char *szPat, size_t cPat)
{
	size_t i;
	size_t j;
	int aLps[B1222_MAX_PAT];

	if (cPat > (size_t)B1222_MAX_PAT) {
		return -1L;
	}
	if (cPat == 0u) {
		if (cHay > 0u && szHay == NULL) {
			return -1L;
		}
		if (szPat == NULL && cPat != 0u) {
			return -1L;
		}
		return 0L;
	}
	if (szHay == NULL || szPat == NULL) {
		return -1L;
	}
	if (cPat > cHay) {
		return -1L;
	}

	(void)b1222_build_lps(szPat, cPat, aLps);

	i = 0u;
	j = 0u;
	while (i < cHay) {
		if (szHay[i] == szPat[j]) {
			i++;
			j++;
			if (j == cPat) {
				return (long)(i - cPat);
			}
		} else if (j != 0u) {
			j = (size_t)aLps[j - 1u];
		} else {
			i++;
		}
	}
	return -1L;
}

/* ---- underscored alias ------------------------------------------------- */

long __gj_kmp_search_first(const char *szHay, size_t cHay,
    const char *szPat, size_t cPat)
    __attribute__((alias("gj_kmp_search_first")));
