/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1223: Boyer–Moore bad-character table build.
 *
 * Surface (unique symbols):
 *   int gj_boyer_moore_badchar_build(const char *pat, size_t n, int *bc);
 *     — Fill bc[0..255] for pattern pat[0..n). Each bc[c] is the
 *       rightmost index of byte c in the pattern, or -1 if absent.
 *       n must be <= 4096. Returns 0 on success; -1 on NULL pat with
 *       n>0, NULL bc, or n > 4096. n==0 zeros all slots to -1.
 *   int __gj_boyer_moore_badchar_build  (alias)
 *   __libcgj_batch1223_marker = "libcgj-batch1223"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

enum { B1223_MAX_N = 4096, B1223_ALPH = 256 };

const char __libcgj_batch1223_marker[] = "libcgj-batch1223";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_boyer_moore_badchar_build — rightmost-occurrence bad-char table.
 */
int
gj_boyer_moore_badchar_build(const char *szPat, size_t cN, int *pBc)
{
	size_t i;
	unsigned uC;

	if (pBc == NULL) {
		return -1;
	}
	if (cN > (size_t)B1223_MAX_N) {
		return -1;
	}
	if (cN > 0u && szPat == NULL) {
		return -1;
	}

	for (i = 0u; i < (size_t)B1223_ALPH; i++) {
		pBc[i] = -1;
	}
	for (i = 0u; i < cN; i++) {
		uC = (unsigned char)szPat[i];
		pBc[uC] = (int)i;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_boyer_moore_badchar_build(const char *szPat, size_t cN, int *pBc)
    __attribute__((alias("gj_boyer_moore_badchar_build")));
