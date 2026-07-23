/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1042: three-component path join (str surface).
 *
 * Surface (unique symbols):
 *   int gj_str_join_path3(const char *a, const char *b, const char *c,
 *                         char *out, size_t cap);
 *     — Join a, b, c into out with '/' between non-empty components.
 *       Skips an extra '/' when a component already ends with '/'.
 *       Returns 0 (OK) on success; -1 (FAIL) on NULL out, cap == 0,
 *       length overflow, or insufficient cap. NULL a/b/c treated as "".
 *   int __gj_str_join_path3  (alias)
 *   __libcgj_batch1042_marker = "libcgj-batch1042"
 *
 * Does NOT redefine gj_path_join / gj_path_join2 / gj_str_join3 /
 * gj_str_join_path2 — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1042_marker[] = "libcgj-batch1042";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1042_len(const char *sz)
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
b1042_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

static int
b1042_add(size_t *pCb, size_t cbAdd)
{
	if (cbAdd > (size_t)-1 - *pCb) {
		return -1;
	}
	*pCb += cbAdd;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_join_path3 — join a, b, c with '/' separators into out.
 *
 * Empty components contribute nothing and do not force extra slashes.
 * When the output built so far is non-empty and does not end with '/',
 * a single '/' is inserted before the next non-empty component.
 */
int
gj_str_join_path3(const char *a, const char *b, const char *c, char *out,
    size_t cap)
{
	const char *asz[3];
	size_t an[3];
	size_t nNeed;
	size_t nOut;
	size_t iPart;
	size_t i;
	int fNeedSep;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	asz[0] = a;
	asz[1] = b;
	asz[2] = c;
	an[0] = b1042_len(a);
	an[1] = b1042_len(b);
	an[2] = b1042_len(c);

	/* Estimate capacity: sum of lengths + at most 2 separators + NUL. */
	nNeed = 0u;
	for (i = 0u; i < 3u; i++) {
		if (b1042_add(&nNeed, an[i]) != 0) {
			return -1;
		}
	}
	if (b1042_add(&nNeed, 2u) != 0) {
		return -1;
	}
	if (b1042_add(&nNeed, 1u) != 0) {
		return -1;
	}
	if (nNeed > cap) {
		/* Still may fit if fewer seps; compute exact below. */
	}

	nOut = 0u;
	fNeedSep = 0;
	for (iPart = 0u; iPart < 3u; iPart++) {
		size_t nPart = an[iPart];
		const char *szPart = asz[iPart];
		int fSep;

		if (nPart == 0u) {
			continue;
		}

		fSep = 0;
		if (fNeedSep) {
			/* Skip sep if previous already ends with '/'. */
			if (nOut > 0u && out[nOut - 1u] == '/') {
				fSep = 0;
			} else if (szPart[0] == '/') {
				/* Next starts with '/'; no extra sep. */
				fSep = 0;
			} else {
				fSep = 1;
			}
		}

		if (nOut + (fSep ? 1u : 0u) + nPart + 1u > cap) {
			return -1;
		}
		if (fSep) {
			out[nOut++] = '/';
		}
		b1042_copy(out + nOut, szPart, nPart);
		nOut += nPart;
		fNeedSep = 1;
	}

	if (nOut + 1u > cap) {
		return -1;
	}
	out[nOut] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_join_path3(const char *a, const char *b, const char *c, char *out,
    size_t cap) __attribute__((alias("gj_str_join_path3")));
