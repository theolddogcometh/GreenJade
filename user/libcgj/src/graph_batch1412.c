/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1412: path stem length (basename without ext).
 *
 * Surface (unique symbols):
 *   size_t gj_path_stem_len_u(const char *path);
 *     — Length of the basename without its final extension. Last path
 *       component only. When there is no extension, returns the full
 *       basename length. Leading-dot hidden names without a further '.'
 *       keep the whole name as the stem.
 *   size_t __gj_path_stem_len_u  (alias)
 *   __libcgj_batch1412_marker = "libcgj-batch1412"
 *
 * NOTE: Symbol names match the batch theme contract. Pre-existing
 * identical surface lives in graph_batch794.c — do not link both TUs
 * into one image without dropping one definition (multi-def).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1412_marker[] = "libcgj-batch1412";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1412_len(const char *sz)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_stem_len_u — length of basename without extension.
 *
 * Examples:
 *   "file.txt"      → 4  ("file")
 *   "a/b/c.tar.gz"  → 7  ("c.tar")
 *   "file."         → 4  ("file")
 *   "file"          → 4  ("file")
 *   ".hidden"       → 7  (".hidden")
 *   ".hidden.txt"   → 7  (".hidden")
 *   "dir/"          → 0
 *   NULL / empty    → 0
 *   "/"             → 1  ("/")
 */
size_t
gj_path_stem_len_u(const char *szPath)
{
	size_t n;
	size_t iEnd;
	size_t iStart;
	size_t i;
	size_t iDot;
	int fDot;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}

	n = b1412_len(szPath);
	iEnd = n;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Pure root. */
	if (iEnd == 1u && szPath[0] == '/') {
		return 1u;
	}

	if (iEnd == 0u) {
		return 0u;
	}

	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (szPath[i] == '/') {
			iStart = i + 1u;
		}
	}

	if (iStart >= iEnd) {
		return 0u;
	}

	/* Find last extension delimiter (skip sole leading-dot). */
	iDot = 0u;
	fDot = 0;
	for (i = iStart; i < iEnd; i++) {
		if (szPath[i] == '.') {
			if (i == iStart) {
				continue;
			}
			iDot = i;
			fDot = 1;
		}
	}

	if (fDot) {
		return iDot - iStart;
	}
	return iEnd - iStart;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_stem_len_u(const char *szPath)
    __attribute__((alias("gj_path_stem_len_u")));
