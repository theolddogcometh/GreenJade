/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch149: unique path string join helper.
 * Clean-room freestanding pure C (integer/pointer; builds with -msse2).
 * No third-party path library source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   pathconf / fpathconf / __pathconf / __fpathconf → syscall_linux.c,
 *     graph_batch21.c
 *   basename / dirname (mutating POSIX)             → string.c
 *   __xpg_basename                                  → graph_batch3.c
 *   basename_r / dirname_r                          → graph_batch35.c
 *   static path_join in ftw.c / fts.c / glob.c      (file-local)
 *   gj_path_basename (pointer, no alloc)            → graph_batch476.c
 *   gj_path_dirname (buffer)                        → graph_batch477.c
 *   gj_path_join2 (a,b,out,cap)                     → graph_batch478.c
 *   gj_path_ext                                     → graph_batch479.c
 *   gj_path_is_abs                                  → graph_batch480.c
 *
 * This TU adds only unique symbols:
 *   int gj_path_join(char *szOut, size_t cbCap,
 *                    const char *szA, const char *szB);
 *   __gj_path_join  (alias)
 *   __libcgj_batch149_marker = "libcgj-batch149"
 *
 * Return convention:
 *   0  success (NUL-terminated result written to szOut)
 *  -1  error (NULL args, cbCap == 0, or result does not fit)
 *
 * Semantics (POSIX-shaped, non-mutating, buffer-safe):
 *   join: concatenate szA and szB with a single '/' when needed.
 *         Empty szA → copy szB; empty szB → copy szA.
 *         If szA already ends with '/', do not insert another.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch149_marker[] = "libcgj-batch149";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b149_len(const char *sz)
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

static void
b149_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

/*
 * Write szSrc[0..nSrc) into szOut with trailing NUL when cbCap is large
 * enough. Returns 0 on success, -1 if it does not fit.
 */
static int
b149_put(char *szOut, size_t cbCap, const char *szSrc, size_t nSrc)
{
	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (nSrc + 1u > cbCap) {
		return -1;
	}
	if (nSrc > 0u && szSrc != NULL) {
		b149_copy(szOut, szSrc, nSrc);
	}
	szOut[nSrc] = '\0';
	return 0;
}

/* ---- gj_path_join ------------------------------------------------------ */

/*
 * Join directory and name components into szOut. Does not resolve ".." or
 * collapse repeated slashes beyond the single separator between parts.
 */
int
gj_path_join(char *szOut, size_t cbCap, const char *szA, const char *szB)
{
	size_t nA;
	size_t nB;
	size_t nNeed;
	size_t i;
	int fSlash;

	if (szOut == NULL || cbCap == 0u || szA == NULL || szB == NULL) {
		return -1;
	}

	nA = b149_len(szA);
	nB = b149_len(szB);

	if (nA == 0u) {
		return b149_put(szOut, cbCap, szB, nB);
	}
	if (nB == 0u) {
		return b149_put(szOut, cbCap, szA, nA);
	}

	fSlash = (szA[nA - 1u] == '/') ? 1 : 0;
	nNeed = nA + nB + (fSlash ? 0u : 1u);
	if (nNeed + 1u > cbCap) {
		return -1;
	}

	b149_copy(szOut, szA, nA);
	i = nA;
	if (!fSlash) {
		szOut[i++] = '/';
	}
	b149_copy(szOut + i, szB, nB);
	szOut[i + nB] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_join(char *szOut, size_t cbCap, const char *szA,
    const char *szB) __attribute__((alias("gj_path_join")));
