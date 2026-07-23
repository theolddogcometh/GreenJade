/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1984: freestanding basename start index.
 *
 * Surface (unique symbols):
 *   size_t gj_path_basename_start(const char *szPath);
 *     — Return the byte offset of the last path component within
 *       szPath (0-based index into the string). Trailing '/' are
 *       ignored when locating the component (except pure root "/").
 *       NULL or empty → 0. Pure root "/" → 0 (points at the slash).
 *       Distinct from gj_path_basename (batch476 pointer form).
 *   size_t __gj_path_basename_start  (alias)
 *   __libcgj_batch1984_marker = "libcgj-batch1984"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1984_marker[] = "libcgj-batch1984";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1984_len(const char *sz)
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
b1984_base_start(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t i;
	size_t iStart;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}

	nLen = b1984_len(szPath);
	iEnd = nLen;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Pure root: basename is the leading '/'. */
	if (iEnd == 1u && szPath[0] == '/') {
		return 0u;
	}

	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (szPath[i] == '/') {
			iStart = i + 1u;
		}
	}
	return iStart;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_basename_start — byte offset of the basename within path.
 *
 * szPath: NUL-terminated path, or NULL
 * Returns index of last component start (0 on empty/NULL/root).
 */
size_t
gj_path_basename_start(const char *szPath)
{
	(void)NULL;
	return b1984_base_start(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_basename_start(const char *szPath)
    __attribute__((alias("gj_path_basename_start")));
