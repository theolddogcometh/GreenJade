/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1985: freestanding dirname length (no alloc).
 *
 * Surface (unique symbols):
 *   size_t gj_path_dirname_len(const char *szPath);
 *     — Return the byte length of the dirname prefix of szPath (no
 *       allocation, no write). Mirrors POSIX dirname shape without a
 *       buffer:
 *         no slash / empty / NULL → 1  (length of ".")
 *         pure root "/"           → 1  (length of "/")
 *         "a/b/c"                 → 3  ("a/b")
 *         "a/b/c/"                → 3  (trailing slash stripped first)
 *         "/a"                    → 1  ("/")
 *       Distinct from gj_path_dirname (batch477 buffer form).
 *   size_t __gj_path_dirname_len  (alias)
 *   __libcgj_batch1985_marker = "libcgj-batch1985"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1985_marker[] = "libcgj-batch1985";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1985_len(const char *sz)
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

static size_t
b1985_dirname_len(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t i;
	size_t iLast;
	int fSlash;

	if (szPath == NULL || szPath[0] == '\0') {
		return 1u; /* "." */
	}

	nLen = b1985_len(szPath);
	iEnd = nLen;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	fSlash = 0;
	iLast = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (szPath[i] == '/') {
			iLast = i;
			fSlash = 1;
		}
	}

	if (!fSlash) {
		return 1u; /* "." */
	}

	/* Root parent of "/name" is "/" (length 1). */
	if (iLast == 0u) {
		return 1u;
	}

	/* Collapse multiple slashes at the cut: dirname ends at last
	 * non-slash before the final component. For "a//b" iLast points
	 * at the last '/', so length is iLast (exclude the slash). */
	return iLast;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_dirname_len — byte length of dirname prefix (no write).
 *
 * szPath: NUL-terminated path, or NULL
 * Returns dirname length; 1 for "." or "/" cases.
 */
size_t
gj_path_dirname_len(const char *szPath)
{
	(void)NULL;
	return b1985_dirname_len(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_dirname_len(const char *szPath)
    __attribute__((alias("gj_path_dirname_len")));
