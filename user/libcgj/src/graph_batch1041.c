/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1041: two-component path join (str surface).
 *
 * Surface (unique symbols):
 *   int gj_str_join_path2(const char *a, const char *b, char *out, size_t cap);
 *     — Concatenate a and b into out with a single '/' when needed.
 *       Argument order is (a, b, out, cap). Returns 0 (OK) on success,
 *       -1 (FAIL) on NULL a/b/out, cap == 0, or result does not fit.
 *   int __gj_str_join_path2  (alias)
 *   __libcgj_batch1041_marker = "libcgj-batch1041"
 *
 * Does NOT redefine gj_path_join / gj_path_join2 / gj_str_join2 — avoid
 * multi-def. Clean-room freestanding pure C (integer/pointer only).
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc,
 * no errno, no libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1041_marker[] = "libcgj-batch1041";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1041_len(const char *sz)
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
b1041_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

static int
b1041_put(char *out, size_t cap, const char *szSrc, size_t nSrc)
{
	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (nSrc + 1u > cap) {
		return -1;
	}
	if (nSrc > 0u && szSrc != NULL) {
		b1041_copy(out, szSrc, nSrc);
	}
	out[nSrc] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_join_path2 — join a and b into out with '/' separator.
 *
 * Edge cases:
 *   a == NULL || b == NULL || out == NULL || cap == 0 → -1 (FAIL)
 *   a empty → copy b; b empty → copy a
 *   a ends with '/' → no extra separator
 */
int
gj_str_join_path2(const char *a, const char *b, char *out, size_t cap)
{
	size_t nA;
	size_t nB;
	size_t nNeed;
	size_t i;
	int fSlash;

	if (a == NULL || b == NULL || out == NULL || cap == 0u) {
		return -1;
	}

	nA = b1041_len(a);
	nB = b1041_len(b);

	if (nA == 0u) {
		return b1041_put(out, cap, b, nB);
	}
	if (nB == 0u) {
		return b1041_put(out, cap, a, nA);
	}

	fSlash = (a[nA - 1u] == '/') ? 1 : 0;
	nNeed = nA + nB + (fSlash ? 0u : 1u);
	if (nNeed + 1u < nNeed || nNeed + 1u > cap) {
		return -1;
	}

	b1041_copy(out, a, nA);
	i = nA;
	if (!fSlash) {
		out[i++] = '/';
	}
	b1041_copy(out + i, b, nB);
	out[i + nB] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_join_path2(const char *a, const char *b, char *out, size_t cap)
    __attribute__((alias("gj_str_join_path2")));
