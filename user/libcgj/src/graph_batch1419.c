/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1419: URI path-start index.
 *
 * Surface (unique symbols):
 *   size_t gj_uri_path_start(const char *uri);
 *     — Index of the path start after scheme and optional authority
 *       (RFC 3986 absolute-URI subset):
 *         scheme ":" [ "//" authority ] path
 *       If no valid scheme, path is treated as starting at index 0.
 *       Authority ends at '/', '?', '#', or NUL. Returns string length
 *       when the path is empty (e.g. "http://host").
 *   size_t __gj_uri_path_start  (alias)
 *   __libcgj_batch1419_marker = "libcgj-batch1419"
 *
 * Distinct from gj_url_parse (batch103) — unique index-only surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1419_marker[] = "libcgj-batch1419";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1419_is_alpha(unsigned char uCh)
{
	return (uCh >= 'A' && uCh <= 'Z') || (uCh >= 'a' && uCh <= 'z');
}

static int
b1419_is_scheme_cont(unsigned char uCh)
{
	return b1419_is_alpha(uCh) ||
	       (uCh >= '0' && uCh <= '9') ||
	       uCh == '+' || uCh == '-' || uCh == '.';
}

static size_t
b1419_len(const char *sz)
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
 * gj_uri_path_start — byte index where the path component begins.
 *
 * Examples:
 *   "http://ex.com/a"  → index of "/a"
 *   "http://ex.com"    → length (empty path)
 *   "http://ex.com?q"  → index of "?q" (empty path-abempty before query)
 *   "mailto:user@h"    → index of "user@h" (no authority)
 *   "file:///tmp"      → index of "/tmp"
 *   "/abs"             → 0
 *   "rel"              → 0
 *   NULL               → 0
 */
size_t
gj_uri_path_start(const char *szUri)
{
	size_t i;
	size_t n;

	if (szUri == NULL || szUri[0] == '\0') {
		return 0u;
	}

	n = b1419_len(szUri);

	/* Detect scheme = ALPHA *(scheme-cont) ":" */
	if (!b1419_is_alpha((unsigned char)szUri[0])) {
		return 0u;
	}

	i = 1u;
	while (i < n && b1419_is_scheme_cont((unsigned char)szUri[i])) {
		i++;
	}

	if (i >= n || szUri[i] != ':') {
		/* No scheme — whole string is path-ish. */
		return 0u;
	}

	/* Skip ':' */
	i++;

	/* Optional "//" authority */
	if (i + 1u < n && szUri[i] == '/' && szUri[i + 1u] == '/') {
		i += 2u;
		while (i < n &&
		       szUri[i] != '/' &&
		       szUri[i] != '?' &&
		       szUri[i] != '#') {
			i++;
		}
		return i;
	}

	/* Hier-part without authority: path starts right after scheme. */
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_uri_path_start(const char *szUri)
    __attribute__((alias("gj_uri_path_start")));
