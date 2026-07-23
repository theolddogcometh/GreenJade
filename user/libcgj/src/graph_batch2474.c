/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2474: freestanding dual-sep path basename.
 *
 * Surface (unique symbols):
 *   const char *gj_path_basename_u(const char *path);
 *     — Pointer to the last path component of path (within path itself).
 *       Accepts both '/' and '\\' as separators. No allocation.
 *       Trailing separators are stripped when locating the component.
 *       A path of only separators yields a pointer to path[0].
 *       NULL or empty path → stable empty-string sentinel.
 *   const char *__gj_path_basename_u  (alias)
 *   __libcgj_batch2474_marker = "libcgj-batch2474"
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_basename
 * (batch476, '/' only) — unique _u dual-separator surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2474_marker[] = "libcgj-batch2474";

/* Empty-string sentinel for NULL / empty inputs (stable address). */
static const char s_b2474_empty[] = "";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2474_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static size_t
b2474_len(const char *sz)
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

static const char *
b2474_basename(const char *szPath)
{
	size_t n;
	size_t iEnd;
	size_t i;
	size_t iStart;

	if (szPath == NULL || szPath[0] == '\0') {
		return s_b2474_empty;
	}

	n = b2474_len(szPath);
	iEnd = n;
	/* Strip trailing separators, but keep a single root separator. */
	while (iEnd > 1u && b2474_is_sep(szPath[iEnd - 1u])) {
		iEnd--;
	}

	/* Only separators (root-like). */
	if (iEnd == 1u && b2474_is_sep(szPath[0])) {
		return szPath;
	}

	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (b2474_is_sep(szPath[i])) {
			iStart = i + 1u;
		}
	}

	return szPath + iStart;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_basename_u — last component; pointer into path (dual sep).
 *
 * Edge cases:
 *   path == NULL          → ""
 *   path == ""            → ""
 *   path == "/"           → "/"
 *   path == "\\"          → "\\"
 *   path == "a/b\\c"      → "c"
 *   path == "a\\b\\c\\"   → "c"
 *   path == "foo"         → "foo"
 *
 * Returned pointer is either s_b2474_empty or an address within path.
 */
const char *
gj_path_basename_u(const char *szPath)
{
	(void)NULL;
	return b2474_basename(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_path_basename_u(const char *szPath)
    __attribute__((alias("gj_path_basename_u")));
