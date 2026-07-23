/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2169: freestanding path stem start index.
 *
 * Surface (unique symbols):
 *   size_t gj_path_stem_start(const char *path);
 *     - Return the byte offset of the file stem within path. The stem
 *       is the basename without its final extension: same start as the
 *       basename component. Trailing '/' are ignored when locating it
 *       (except pure root "/"). NULL or empty -> 0. Pure root "/" -> 0.
 *       Pair with gj_path_ext_start (batch2168): stem spans
 *       [stem_start, ext_start).
 *   size_t __gj_path_stem_start  (alias)
 *   __libcgj_batch2169_marker = "libcgj-batch2169"
 *
 * Post-2160 path exclusive wave (2161-2170). Distinct from
 * gj_path_basename_start (batch1984) - stem-named surface for this wave;
 * no multi-def of prior symbols.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2169_marker[] = "libcgj-batch2169";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2169_len(const char *sz)
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
b2169_stem_start(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t i;
	size_t iStart;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0u;
	}

	nLen = b2169_len(szPath);
	iEnd = nLen;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Pure root: stem is the leading '/'. */
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
 * gj_path_stem_start - byte offset of the stem (basename start).
 *
 * path: NUL-terminated path, or NULL
 *
 * Returns index of stem start (0 on empty/NULL/root). Stem ends at
 * gj_path_ext_start (batch2168).
 */
size_t
gj_path_stem_start(const char *path)
{
	(void)NULL;
	return b2169_stem_start(path);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_path_stem_start(const char *path)
    __attribute__((alias("gj_path_stem_start")));
