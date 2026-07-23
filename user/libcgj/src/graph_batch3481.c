/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3481: freestanding path basename length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_path_basename_len_u(const char *path);
 *     - Byte length of the last path component (no allocation). Accepts
 *       both '/' and '\\' as separators. Trailing separators are stripped
 *       before measuring (except pure root, which yields length 1).
 *       NULL or empty -> 0.
 *   size_t __gj_path_basename_len_u  (alias)
 *   __libcgj_batch3481_marker = "libcgj-batch3481"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Distinct from
 * gj_path_basename (batch476 pointer), gj_path_basename_start
 * (batch1984 offset), and gj_path_stem_len_u (batch1412). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3481_marker[] = "libcgj-batch3481";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3481_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static size_t
b3481_len(const char *sz)
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
b3481_basename_len(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t i;
	size_t iStart;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}

	nLen = b3481_len(szPath);
	iEnd = nLen;
	/* Strip trailing seps, but keep a single root separator. */
	while (iEnd > 1u && b3481_is_sep(szPath[iEnd - 1u])) {
		iEnd--;
	}

	/* Pure root: basename length is 1. */
	if (iEnd == 1u && b3481_is_sep(szPath[0])) {
		return 1u;
	}

	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (b3481_is_sep(szPath[i])) {
			iStart = i + 1u;
		}
	}
	return iEnd - iStart;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_basename_len_u - byte length of the last path component.
 *
 * path: NUL-terminated path, or NULL
 *
 * Examples:
 *   "a/b/c"   -> 1
 *   "a/b/c/"  -> 1
 *   "foo"     -> 3
 *   "/"       -> 1
 *   "a\\b\\cd"-> 2
 *   NULL / "" -> 0
 *
 * Length-only; does not allocate. Does not call libc. No parent wires.
 */
size_t
gj_path_basename_len_u(const char *szPath)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3481_basename_len(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_basename_len_u(const char *szPath)
    __attribute__((alias("gj_path_basename_len_u")));
