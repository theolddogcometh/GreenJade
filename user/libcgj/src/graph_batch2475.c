/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2475: freestanding dual-sep dirname buffer copy.
 *
 * Surface (unique symbols):
 *   int gj_path_dirname_copy_u(const char *path, char *out, size_t cap);
 *     — Write the parent directory of path into out (capacity cap,
 *       including trailing NUL). Accepts both '/' and '\\' as separators.
 *       Returns 0 on success, -1 on error (NULL out, cap == 0, or
 *       result including NUL does not fit). On error out is undefined.
 *   int __gj_path_dirname_copy_u  (alias)
 *   __libcgj_batch2475_marker = "libcgj-batch2475"
 *
 * Edge cases (mirror POSIX dirname shape with dual separators):
 *   NULL / empty path → "."
 *   no separator      → "."
 *   root component    → "/"  (normalized to forward slash)
 *   trailing seps stripped except pure root.
 *
 * Path exclusive wave (2471–2480). Distinct from gj_path_dirname
 * (batch477, '/' only) — unique _copy_u dual-separator surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2475_marker[] = "libcgj-batch2475";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2475_is_sep(char ch)
{
	return (ch == '/' || ch == '\\') ? 1 : 0;
}

static size_t
b2475_len(const char *sz)
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
b2475_copy(char *szDst, const char *szSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		szDst[i] = szSrc[i];
	}
}

static int
b2475_put(char *szOut, size_t cbCap, const char *szSrc, size_t nSrc)
{
	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (nSrc + 1u > cbCap) {
		return -1;
	}
	if (nSrc > 0u && szSrc != NULL) {
		b2475_copy(szOut, szSrc, nSrc);
	}
	szOut[nSrc] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_dirname_copy_u — parent directory into caller buffer (dual sep).
 *
 * path: source path (may be NULL → treat as empty → ".")
 * out:  destination for NUL-terminated dirname
 * cap:  capacity of out in bytes
 *
 * Returns 0 / -1. No errno (freestanding).
 */
int
gj_path_dirname_copy_u(const char *szPath, char *szOut, size_t cbCap)
{
	size_t n;
	size_t iEnd;
	size_t i;
	size_t iLast = 0u;
	int fSep = 0;

	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}

	if (szPath == NULL || szPath[0] == '\0') {
		return b2475_put(szOut, cbCap, ".", 1u);
	}

	n = b2475_len(szPath);
	iEnd = n;
	while (iEnd > 1u && b2475_is_sep(szPath[iEnd - 1u])) {
		iEnd--;
	}

	for (i = 0u; i < iEnd; i++) {
		if (b2475_is_sep(szPath[i])) {
			iLast = i;
			fSep = 1;
		}
	}

	if (!fSep) {
		return b2475_put(szOut, cbCap, ".", 1u);
	}
	if (iLast == 0u) {
		/* Root parent: normalize to '/'. */
		return b2475_put(szOut, cbCap, "/", 1u);
	}

	return b2475_put(szOut, cbCap, szPath, iLast);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_dirname_copy_u(const char *szPath, char *szOut, size_t cbCap)
    __attribute__((alias("gj_path_dirname_copy_u")));
