/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch682: freestanding three-string join.
 *
 * Surface (unique symbols):
 *   int gj_str_join3(const char *a, const char *b, const char *c,
 *                    char *out, size_t cap);
 *     — Concatenate a, b, and c (in that order, no separator) into out.
 *       Writes a NUL-terminated result when the full string fits in cap
 *       bytes (including the terminator). Returns 0 (OK) on success;
 *       -1 (FAIL) on NULL out, cap == 0, size overflow of total length,
 *       or insufficient cap. NULL a/b/c are treated as empty ("").
 *       On failure out is left unspecified.
 *   int __gj_str_join3  (alias)
 *   __libcgj_batch682_marker = "libcgj-batch682"
 *
 * Distinct from gj_path_join / gj_path_join2 (path '/' separator) and
 * gj_join (array + sep). Clean-room freestanding pure C (integer/pointer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc string helpers. No third-party source
 * copied.
 */

#include <stddef.h>

const char __libcgj_batch682_marker[] = "libcgj-batch682";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b682_len(const char *sz)
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
b682_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

/*
 * Add cbAdd into *pCb. Returns 0 on success, -1 if the sum would wrap
 * size_t.
 */
static int
b682_add(size_t *pCb, size_t cbAdd)
{
	if (cbAdd > (size_t)-1 - *pCb) {
		return -1;
	}
	*pCb += cbAdd;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_join3 — concatenate a, b, c into out (no separator).
 *
 * a, b, c: source C strings (NULL → empty)
 * out:     destination buffer
 * cap:     capacity of out including space for the terminating NUL
 *
 * Returns 0 (OK) when out is NUL-terminated with a||b||c; -1 (FAIL)
 * otherwise. On FAIL out contents are unspecified.
 */
int
gj_str_join3(const char *a, const char *b, const char *c, char *out,
    size_t cap)
{
	size_t nA;
	size_t nB;
	size_t nC;
	size_t nNeed;
	size_t i;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	nA = b682_len(a);
	nB = b682_len(b);
	nC = b682_len(c);

	/* Total payload length (excluding NUL), overflow-safe. */
	nNeed = 0u;
	if (b682_add(&nNeed, nA) != 0 ||
	    b682_add(&nNeed, nB) != 0 ||
	    b682_add(&nNeed, nC) != 0) {
		return -1;
	}

	/* Need room for payload + terminating NUL. */
	if (nNeed + 1u < nNeed || nNeed + 1u > cap) {
		return -1;
	}

	i = 0u;
	if (nA > 0u) {
		b682_copy(out + i, a, nA);
		i += nA;
	}
	if (nB > 0u) {
		b682_copy(out + i, b, nB);
		i += nB;
	}
	if (nC > 0u) {
		b682_copy(out + i, c, nC);
		i += nC;
	}
	out[i] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_join3(const char *a, const char *b, const char *c, char *out,
    size_t cap) __attribute__((alias("gj_str_join3")));
