/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1221: KMP longest-prefix-suffix (LPS) table build.
 *
 * Surface (unique symbols):
 *   int gj_kmp_build_lps(const char *pat, size_t n, int *lps);
 *     — Fill lps[0..n) for pattern pat[0..n). lps[i] is the longest
 *       proper prefix of pat[0..i] that is also a suffix of pat[0..i].
 *       n must be <= 4096. Returns 0 on success; -1 on NULL pat/lps
 *       with n>0, or n > 4096. n==0 → 0 (no writes).
 *   int __gj_kmp_build_lps  (alias)
 *   __libcgj_batch1221_marker = "libcgj-batch1221"
 *
 * Distinct from gj_kmp_find (batch216) which searches; this only builds
 * the π / LPS table. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1221_MAX_N = 4096 };

const char __libcgj_batch1221_marker[] = "libcgj-batch1221";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kmp_build_lps — classic KMP LPS / π table for pat[0..n).
 */
int
gj_kmp_build_lps(const char *szPat, size_t cN, int *pLps)
{
	size_t i;
	size_t cLen;

	if (cN > (size_t)B1221_MAX_N) {
		return -1;
	}
	if (cN == 0u) {
		return 0;
	}
	if (szPat == NULL || pLps == NULL) {
		return -1;
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_kmp_build_lps(const char *szPat, size_t cN, int *pLps)
    __attribute__((alias("gj_kmp_build_lps")));
