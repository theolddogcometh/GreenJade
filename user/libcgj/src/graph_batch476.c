/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch476: freestanding path basename (no alloc).
 *
 * Surface (unique symbols):
 *   const char *gj_path_basename(const char *path);
 *     — Pointer to the last path component of path (within path itself).
 *       No allocation; never returns a pointer into static storage except
 *       the empty-string sentinel when path is NULL or empty after strip.
 *       Trailing '/' are skipped when locating the component. A path of
 *       only '/' yields a pointer to the leading '/'. NULL path → "".
 *   const char *__gj_path_basename  (alias)
 *   __libcgj_batch476_marker = "libcgj-batch476"
 *
 * Distinct from the buffer-copy basename surface historically in
 * batch149 (removed; this pointer form is exclusive here).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch476_marker[] = "libcgj-batch476";

/* Empty-string sentinel for NULL / empty inputs (stable address). */
static const char s_b476_empty[] = "";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b476_len(const char *sz)
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
 * gj_path_basename — last path component; pointer into path (no alloc).
 *
 * Edge cases:
 *   path == NULL          → ""
 *   path == ""            → ""
 *   path == "/"           → "/"  (pointer to path[0])
 *   path == "///"         → "/"  (pointer to path[0])
 *   path == "foo"         → "foo"
 *   path == "a/b/c"       → "c"
 *   path == "a/b/c/"      → "c"  (trailing slash stripped for search)
 *   path == "a/b/c///"    → "c"
 *
 * The returned pointer is either s_b476_empty or an address within path.
 * Callers must not free it. Mutating path invalidates the result.
 */
const char *
gj_path_basename(const char *path)
{
	size_t n;
	size_t iEnd;
	size_t i;
	size_t iStart;

	if (path == NULL || path[0] == '\0') {
		return s_b476_empty;
	}

	n = b476_len(path);
	iEnd = n;
	/* Strip trailing slashes, but keep a single root slash. */
	while (iEnd > 1u && path[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Only slashes (root). */
	if (iEnd == 1u && path[0] == '/') {
		return path;
	}

	iStart = 0u;
	for (i = 0u; i < iEnd; i++) {
		if (path[i] == '/') {
			iStart = i + 1u;
		}
	}

	return path + iStart;
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_path_basename(const char *path)
    __attribute__((alias("gj_path_basename")));
