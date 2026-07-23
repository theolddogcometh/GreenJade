/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2828: two-component path join byte need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_path_join_need_u(const char *a, const char *b);
 *     - Byte length of the string that would result from joining a and
 *       b with a single '/' when needed (including the trailing NUL).
 *       Empty / NULL components contribute nothing and do not force
 *       extra separators. If a already ends with '/', no separator is
 *       inserted before b. Empty join (both NULL/empty) returns 1
 *       (just the trailing NUL).
 *   size_t __gj_path_join_need_u  (alias)
 *   __libcgj_batch2828_marker = "libcgj-batch2828"
 *
 * ASCII/path exclusive wave (2821-2830). Distinct from gj_path_join
 * (batch149 write), gj_path_join2 (batch478 write), and
 * gj_path_join3_len_u (batch2476 three-way) — two-arg length-only
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2828_marker[] = "libcgj-batch2828";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2828_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}
	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Bytes needed (incl. trailing NUL) to join a and b with '/' as needed.
 */
static size_t
b2828_join_need(const char *szA, const char *szB)
{
	size_t nA;
	size_t nB;
	size_t nOut;
	int fSlash;

	nA = b2828_len(szA);
	nB = b2828_len(szB);

	if (nA == 0u && nB == 0u) {
		return 1u; /* "" + NUL */
	}
	if (nA == 0u) {
		return nB + 1u;
	}
	if (nB == 0u) {
		return nA + 1u;
	}

	fSlash = (szA[nA - 1u] == '/') ? 1 : 0;
	nOut = nA + nB;
	if (fSlash == 0) {
		nOut = nOut + 1u; /* inserted '/' */
	}
	return nOut + 1u; /* trailing NUL */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_join_need_u - bytes needed (incl. NUL) for a two-way path join.
 *
 * a: first path component (NULL treated as empty)
 * b: second path component (NULL treated as empty)
 *
 * Examples (return includes trailing NUL):
 *   ("a","b")   → 4  ("a/b" + NUL)
 *   ("a/","b")  → 4  ("a/b" + NUL)
 *   ("","")     → 1  ("" + NUL)
 *   (NULL,"x")  → 2  ("x" + NUL)
 *   ("a","")    → 2  ("a" + NUL)
 *
 * Length-only; does not write a buffer. Does not call libc.
 */
size_t
gj_path_join_need_u(const char *szA, const char *szB)
{
	(void)NULL;
	return b2828_join_need(szA, szB);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_join_need_u(const char *szA, const char *szB)
    __attribute__((alias("gj_path_join_need_u")));
