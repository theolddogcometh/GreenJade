/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1982: freestanding Unix hidden-basename probe.
 *
 * Surface (unique symbols):
 *   int gj_path_is_hidden_unix(const char *szPath);
 *     — Return 1 if the basename of szPath is a Unix hidden name: it
 *       begins with '.' and is not the special components "." or "..".
 *       Trailing '/' are stripped before basename selection (except a
 *       pure-root path, which is not hidden). NULL or empty → 0.
 *   int __gj_path_is_hidden_unix  (alias)
 *   __libcgj_batch1982_marker = "libcgj-batch1982"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1982_marker[] = "libcgj-batch1982";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1982_len(const char *sz)
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

/*
 * Index of basename start after stripping trailing slashes (keep root).
 * Returns 0 on empty/NULL-like inputs.
 */
static size_t
b1982_base_start(const char *szPath, size_t nLen)
{
	size_t iEnd;
	size_t i;
	size_t iStart;

	if (nLen == 0u) {
		return 0u;
	}

	iEnd = nLen;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}

	/* Pure root "/" (or "///" reduced) — no hidden basename. */
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

/* 1 if basename [iStart, iEnd) is hidden (leading '.' not . or ..). */
static int
b1982_hidden(const char *szPath, size_t iStart, size_t iEnd)
{
	size_t cbBase;

	if (iEnd <= iStart) {
		return 0;
	}
	if (szPath[iStart] != '.') {
		return 0;
	}

	cbBase = iEnd - iStart;
	/* "." alone — not a hidden file name. */
	if (cbBase == 1u) {
		return 0;
	}
	/* ".." alone — not a hidden file name. */
	if (cbBase == 2u && szPath[iStart + 1u] == '.') {
		return 0;
	}
	return 1;
}

static int
b1982_probe(const char *szPath)
{
	size_t nLen;
	size_t iEnd;
	size_t iStart;

	if (szPath == NULL || szPath[0] == '\0') {
		return 0;
	}

	nLen = b1982_len(szPath);
	iEnd = nLen;
	while (iEnd > 1u && szPath[iEnd - 1u] == '/') {
		iEnd--;
	}
	if (iEnd == 1u && szPath[0] == '/') {
		return 0;
	}

	iStart = b1982_base_start(szPath, nLen);
	return b1982_hidden(szPath, iStart, iEnd);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_is_hidden_unix — 1 if basename is a Unix hidden name.
 *
 * szPath: NUL-terminated path, or NULL
 * Returns 1 when hidden, else 0.
 */
int
gj_path_is_hidden_unix(const char *szPath)
{
	(void)NULL;
	return b1982_probe(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_is_hidden_unix(const char *szPath)
    __attribute__((alias("gj_path_is_hidden_unix")));
