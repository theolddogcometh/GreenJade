/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3482: freestanding path dirname length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_path_dirname_len_u(const char *path);
 *     - Byte length of the dirname prefix (no allocation, no write).
 *       Accepts both '/' and '\\'. Mirrors POSIX dirname shape:
 *         no sep / empty / NULL -> 1  (length of ".")
 *         pure root "/" or "\\" -> 1
 *         "a/b/c"               -> 3  ("a/b")
 *         "a\\b\\c"             -> 3  ("a\\b")
 *       Distinct from gj_path_dirname_len (batch1985, '/' only).
 *   size_t __gj_path_dirname_len_u  (alias)
 *   __libcgj_batch3482_marker = "libcgj-batch3482"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Unique
 * gj_path_dirname_len_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3482_marker[] = "libcgj-batch3482";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3482_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static size_t
b3482_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}
	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

static size_t
b3482_dirname_len(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t i;
	size_t iLast;
	int fSep;

	if (szPath == NULL || szPath[0] == '\0') {
		return 1u; /* "." */
	}

	nLen = b3482_len(szPath);
	iEnd = nLen;
	while (iEnd > 1u && b3482_is_sep(szPath[iEnd - 1u])) {
		iEnd--;
	}

	fSep = 0;
	iLast = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (b3482_is_sep(szPath[i])) {
			iLast = i;
			fSep = 1;
		}
	}

	if (!fSep) {
		return 1u; /* "." */
	}

	/* Root parent of "/name" or "\\name" is one separator (length 1). */
	if (iLast == 0u) {
		return 1u;
	}

	/* dirname ends at the last non-sep before the final component. */
	return iLast;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_dirname_len_u - byte length of the dirname prefix.
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns dirname length; 1 for "." or root cases. Dual-sep form.
 * Does not call libc. No parent wires.
 */
size_t
gj_path_dirname_len_u(const char *szPath)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3482_dirname_len(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_dirname_len_u(const char *szPath)
    __attribute__((alias("gj_path_dirname_len_u")));
