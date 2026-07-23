/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch478: freestanding two-component path join.
 *
 * Surface (unique symbols):
 *   int gj_path_join2(const char *a, const char *b, char *out, size_t cap);
 *     — Concatenate a and b into out with a single '/' when needed.
 *       Argument order is (a, b, out, cap) — distinct from gj_path_join
 *       in batch149 which uses (out, cap, a, b).
 *       Returns 0 on success, -1 on error (NULL a/b/out, cap == 0, or
 *       result including NUL does not fit). On error out is undefined.
 *   int __gj_path_join2  (alias)
 *   __libcgj_batch478_marker = "libcgj-batch478"
 *
 * Semantics:
 *   empty a → copy b; empty b → copy a.
 *   If a already ends with '/', do not insert another separator.
 *   Does not resolve ".." or collapse repeated slashes beyond the single
 *   separator between the two parts.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch478_marker[] = "libcgj-batch478";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b478_len(const char *sz)
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
b478_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

static int
b478_put(char *out, size_t cap, const char *szSrc, size_t nSrc)
{
	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (nSrc + 1u > cap) {
		return -1;
	}
	if (nSrc > 0u && szSrc != NULL) {
		b478_copy(out, szSrc, nSrc);
	}
	out[nSrc] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_join2 — join a and b into out (a, b, out, cap argument order).
 *
 * Edge cases:
 *   a == NULL || b == NULL || out == NULL || cap == 0 → -1
 *   a empty → copy b
 *   b empty → copy a
 *   a ends with '/' → no extra separator
 */
int
gj_path_join2(const char *a, const char *b, char *out, size_t cap)
{
	size_t nA;
	size_t nB;
	size_t nNeed;
	size_t i;
	int fSlash;

	if (a == NULL || b == NULL || out == NULL || cap == 0u) {
		return -1;
	}

	nA = b478_len(a);
	nB = b478_len(b);

	if (nA == 0u) {
		return b478_put(out, cap, b, nB);
	}
	if (nB == 0u) {
		return b478_put(out, cap, a, nA);
	}

	fSlash = (a[nA - 1u] == '/') ? 1 : 0;
	nNeed = nA + nB + (fSlash ? 0u : 1u);
	if (nNeed + 1u > cap) {
		return -1;
	}

	b478_copy(out, a, nA);
	i = nA;
	if (!fSlash) {
		out[i++] = '/';
	}
	b478_copy(out + i, b, nB);
	out[i + nB] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_join2(const char *a, const char *b, char *out, size_t cap)
    __attribute__((alias("gj_path_join2")));
