/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch681: freestanding two-string concatenate.
 *
 * Surface (unique symbols):
 *   int gj_str_join2(const char *a, const char *b, char *out, size_t cap);
 *     — Concatenate a and b into out (a then b) as a single
 *       NUL-terminated string. No separator is inserted.
 *       Argument order is (a, b, out, cap).
 *       Returns 0 on success (OK), -1 on error (FAIL): NULL a/b/out,
 *       cap == 0, or result including NUL does not fit in cap. On
 *       error out is undefined.
 *   int __gj_str_join2  (alias)
 *   __libcgj_batch681_marker = "libcgj-batch681"
 *
 * Distinct from gj_path_join2 (batch478) which inserts '/' between
 * components, and from gj_path_join (batch149) which uses (out, cap,
 * a, b). Unique gj_ names only; no multi-def with the existing string
 * or path graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch681_marker[] = "libcgj-batch681";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b681_len(const char *sz)
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
b681_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_join2 — concatenate a and b into out (no separator).
 *
 * Edge cases:
 *   a == NULL || b == NULL || out == NULL || cap == 0 → -1 (FAIL)
 *   a empty → copy b (still requires a non-NULL)
 *   b empty → copy a
 *   nA + nB + 1 > cap → -1 (FAIL)
 * Success: 0 (OK). Failure: -1 (FAIL).
 */
int
gj_str_join2(const char *a, const char *b, char *out, size_t cap)
{
	size_t nA;
	size_t nB;
	size_t nNeed;

	if (a == NULL || b == NULL || out == NULL || cap == 0u) {
		return -1;
	}

	nA = b681_len(a);
	nB = b681_len(b);

	/* overflow-safe need: nA + nB + NUL */
	if (nA > cap - 1u) {
		return -1;
	}
	nNeed = nA + nB + 1u;
	if (nNeed < nA || nNeed > cap) {
		return -1;
	}

	b681_copy(out, a, nA);
	b681_copy(out + nA, b, nB);
	out[nA + nB] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_join2(const char *a, const char *b, char *out, size_t cap)
    __attribute__((alias("gj_str_join2")));
