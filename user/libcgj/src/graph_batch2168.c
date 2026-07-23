/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2168: freestanding path extension start index.
 *
 * Surface (unique symbols):
 *   size_t gj_path_ext_start(const char *path);
 *     - Return the byte offset of the final filename extension within
 *       path (index of the last '.' in the basename that starts an
 *       extension, including that '.'). No extension -> string length
 *       (empty extension span). NULL or empty -> 0. Leading-dot-only
 *       basenames (".hidden") have no extension. Trailing '/' are
 *       stripped when locating the basename.
 *   size_t __gj_path_ext_start  (alias)
 *   __libcgj_batch2168_marker = "libcgj-batch2168"
 *
 * Post-2160 path exclusive wave (2161-2170). Distinct from gj_path_ext
 * (batch479 buffer form) - index-only surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2168_marker[] = "libcgj-batch2168";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2168_len(const char *sz)
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
b2168_ext_start(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t iStart;
	size_t i;
	size_t iDot;
	int fDot;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}

	nLen = b2168_len(szPath);
	iEnd = nLen;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Pure root or empty after strip -> no extension; report nLen. */
	if (iEnd == 0u || (iEnd == 1u && szPath[0] == '/')) {
		return nLen;
	}

	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (szPath[i] == '/') {
			iStart = i + 1u;
		}
	}

	if (iStart >= iEnd) {
		return nLen;
	}

	/*
	 * Last '.' in the basename starts the extension, except a lone
	 * leading '.' of a hidden name with no further '.' chars.
	 */
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
		return nLen;
	}
	return iDot;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_ext_start - byte offset of the extension (including '.').
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns index into path, or path length when there is no extension.
 */
size_t
gj_path_ext_start(const char *path)
{
	(void)NULL;
	return b2168_ext_start(path);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_ext_start(const char *path)
    __attribute__((alias("gj_path_ext_start")));
