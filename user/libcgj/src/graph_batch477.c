/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch477: freestanding path dirname (buffer-safe).
 *
 * Surface (unique symbols):
 *   int gj_path_dirname(const char *path, char *out, size_t cap);
 *     — Write the parent directory of path into out (capacity cap,
 *       including trailing NUL). POSIX-shaped, non-mutating.
 *       Returns 0 on success, -1 on error (NULL out, cap == 0, or
 *       result including NUL does not fit). On error out is undefined.
 *   int __gj_path_dirname  (alias)
 *   __libcgj_batch477_marker = "libcgj-batch477"
 *
 * Edge cases (mirror dirname_r / historical batch149):
 *   NULL / empty path → "."
 *   no slash          → "."
 *   root component    → "/"
 *   trailing slashes stripped except for pure root.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch477_marker[] = "libcgj-batch477";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b477_len(const char *sz)
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
b477_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

/*
 * Write szSrc[0..nSrc) into out with trailing NUL when cap is large
 * enough. Returns 0 on success, -1 if it does not fit.
 */
static int
b477_put(char *out, size_t cap, const char *szSrc, size_t nSrc)
{
	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (nSrc + 1u > cap) {
		return -1;
	}
	if (nSrc > 0u && szSrc != NULL) {
		b477_copy(out, szSrc, nSrc);
	}
	out[nSrc] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_dirname — parent directory into caller buffer.
 *
 * path: source path (may be NULL → treat as empty → ".")
 * out:  destination for NUL-terminated dirname
 * cap:  capacity of out in bytes
 *
 * Returns 0 / -1. No errno (freestanding).
 */
int
gj_path_dirname(const char *path, char *out, size_t cap)
{
	size_t n;
	size_t iEnd;
	size_t i;
	size_t iLast = 0u;
	int fSlash = 0;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	if (path == NULL || path[0] == '\0') {
		return b477_put(out, cap, ".", 1u);
	}

	n = b477_len(path);
	iEnd = n;
	while (iEnd > 1u && path[iEnd - 1u] == '/') {
		iEnd--;
	}

	for (i = 0u; i < iEnd; i++) {
		if (path[i] == '/') {
			iLast = i;
			fSlash = 1;
		}
	}

	if (!fSlash) {
		return b477_put(out, cap, ".", 1u);
	}
	if (iLast == 0u) {
		return b477_put(out, cap, "/", 1u);
	}

	return b477_put(out, cap, path, iLast);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_dirname(const char *path, char *out, size_t cap)
    __attribute__((alias("gj_path_dirname")));
