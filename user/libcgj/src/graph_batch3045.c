/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3045: path stem length (basename without ext).
 *
 * Surface (unique symbols):
 *   size_t gj_path_stem_len_u2(const char *path);
 *     - Length of the basename without its final extension. Last path
 *       component only. When there is no extension, returns the full
 *       basename length. Leading-dot hidden names without a further '.'
 *       keep the whole name as the stem. Renamed from gj_path_stem_len_u
 *       (collision with batch1412).
 *   size_t __gj_path_stem_len_u2  (alias)
 *   __libcgj_batch3045_marker = "libcgj-batch3045"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_path_stem_len_u2 surface only; no multi-def. Distinct from
 * gj_path_stem_len_u (batch1412), gj_path_stem_start (batch2169), and
 * gj_path_ext_len_u2 (batch3044). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3045_marker[] = "libcgj-batch3045";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3045_len(const char *sz)
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
 * gj_path_stem_len_u2 - length of basename without extension.
 *
 * Examples:
 *   "file.txt"      -> 4  ("file")
 *   "a/b/c.tar.gz"  -> 7  ("c.tar")
 *   "file."         -> 4  ("file")
 *   "file"          -> 4  ("file")
 *   ".hidden"       -> 7  (".hidden")
 *   ".hidden.txt"   -> 7  (".hidden")
 *   "dir/"          -> 0
 *   NULL / empty    -> 0
 *   "/"             -> 1  ("/")
 *
 * Does not call libc. No parent wires.
 */
size_t
gj_path_stem_len_u2(const char *szPath)
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

	n = b3045_len(szPath);
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

	(void)NULL;
	if (fDot) {
		return iDot - iStart;
	}
	return iEnd - iStart;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_stem_len_u2(const char *szPath)
    __attribute__((alias("gj_path_stem_len_u2")));
