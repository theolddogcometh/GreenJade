/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch793: path extension length (including dot).
 *
 * Surface (unique symbols):
 *   size_t gj_path_ext_len(const char *path);
 *     — Length of the final filename extension including the leading
 *       '.', or 0 if there is no extension. Last path component only
 *       (after final '/'). Leading-dot hidden names without a further
 *       '.' do not count as extensions.
 *   size_t __gj_path_ext_len  (alias)
 *   __libcgj_batch793_marker = "libcgj-batch793"
 *
 * Distinct from gj_path_ext (batch479 buffer write) — unique _len name;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch793_marker[] = "libcgj-batch793";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b793_len(const char *sz)
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
 * gj_path_ext_len — length of extension including '.', or 0.
 *
 * Rules (mirror batch479):
 *   "file.txt"      → 4  (".txt")
 *   "a/b/c.tar.gz"  → 3  (".gz")
 *   "file."         → 1  (".")
 *   "file"          → 0
 *   ".hidden"       → 0
 *   ".hidden.txt"   → 4  (".txt")
 *   NULL / empty    → 0
 */
size_t
gj_path_ext_len(const char *szPath)
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

	n = b793_len(szPath);
	iEnd = n;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	if (iEnd == 0u || (iEnd == 1u && szPath[0] == '/')) {
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

	if (!fDot) {
		return 0u;
	}

	return iEnd - iDot;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_ext_len(const char *szPath)
    __attribute__((alias("gj_path_ext_len")));
