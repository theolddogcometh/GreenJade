/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch479: freestanding path file extension helper.
 *
 * Surface (unique symbols):
 *   int gj_path_ext(const char *path, char *out, size_t cap);
 *     — Write the final filename extension of path into out, including
 *       the leading '.' when an extension is present. If there is no
 *       extension, write the empty string "".
 *       Returns 0 on success, -1 on error (NULL out, cap == 0, or
 *       result including NUL does not fit). On error out is undefined.
 *   int __gj_path_ext  (alias)
 *   __libcgj_batch479_marker = "libcgj-batch479"
 *
 * Extension rules (last path component only; after final '/'):
 *   "file.txt"      → ".txt"
 *   "a/b/c.tar.gz"  → ".gz"
 *   "file."         → "."
 *   "file"          → ""
 *   ".hidden"       → ""   (leading-dot name is not an extension)
 *   ".hidden.txt"   → ".txt"
 *   "dir/"          → ""   (basename after strip is empty → "")
 *   NULL / empty    → ""
 *   Only the last '.' in the basename counts; no '.' → "".
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch479_marker[] = "libcgj-batch479";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b479_len(const char *sz)
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
b479_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

static int
b479_put(char *out, size_t cap, const char *szSrc, size_t nSrc)
{
	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (nSrc + 1u > cap) {
		return -1;
	}
	if (nSrc > 0u && szSrc != NULL) {
		b479_copy(out, szSrc, nSrc);
	}
	out[nSrc] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_ext — final extension including '.', or empty string.
 *
 * path: source path (NULL treated as empty → "")
 * out:  destination for NUL-terminated extension text
 * cap:  capacity of out in bytes
 *
 * Returns 0 / -1. No errno (freestanding).
 */
int
gj_path_ext(const char *path, char *out, size_t cap)
{
	size_t n;
	size_t iEnd;
	size_t iStart;
	size_t i;
	size_t iDot;
	int fDot;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	if (path == NULL || path[0] == '\0') {
		return b479_put(out, cap, "", 0u);
	}

	n = b479_len(path);
	iEnd = n;
	while (iEnd > 1u && path[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Pure root or empty after strip → no extension. */
	if (iEnd == 0u || (iEnd == 1u && path[0] == '/')) {
		return b479_put(out, cap, "", 0u);
	}

	/* Basename span: [iStart, iEnd). */
	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (path[i] == '/') {
			iStart = i + 1u;
		}
	}

	if (iStart >= iEnd) {
		return b479_put(out, cap, "", 0u);
	}

	/*
	 * Find the last '.' in the basename. A leading '.' that is the
	 * first character of the basename (hidden file with no further
	 * '.') does not count as an extension delimiter.
	 */
	iDot = 0u;
	fDot = 0;
	for (i = iStart; i < iEnd; i++) {
		if (path[i] == '.') {
			/* Skip the sole leading-dot of a hidden name. */
			if (i == iStart) {
				continue;
			}
			iDot = i;
			fDot = 1;
		}
	}

	if (!fDot) {
		return b479_put(out, cap, "", 0u);
	}

	/* Extension includes the '.' at iDot through iEnd-1. */
	return b479_put(out, cap, path + iDot, iEnd - iDot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_ext(const char *path, char *out, size_t cap)
    __attribute__((alias("gj_path_ext")));
