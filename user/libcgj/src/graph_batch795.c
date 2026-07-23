/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch795: absolute URL predicate (scheme://).
 *
 * Surface (unique symbols):
 *   int gj_url_is_abs(const char *url);
 *     — Return 1 if url starts with a scheme followed by "://".
 *       Scheme: leading ALPHA, then ALPHA / DIGIT / '+' / '-' / '.'
 *       (RFC 3986 §3.1). Case-sensitive match of the "://" separator.
 *       NULL / empty / no scheme → 0.
 *   int __gj_url_is_abs  (alias)
 *   __libcgj_batch795_marker = "libcgj-batch795"
 *
 * Distinct from gj_url_parse (batch103) — unique predicate; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch795_marker[] = "libcgj-batch795";

/* ---- helpers ----------------------------------------------------------- */

static int
b795_is_alpha(unsigned char u8C)
{
	return ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z')) ? 1 : 0;
}

static int
b795_is_scheme_ch(unsigned char u8C)
{
	if (b795_is_alpha(u8C)) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C == (unsigned char)'+' || u8C == (unsigned char)'-' ||
	    u8C == (unsigned char)'.') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_url_is_abs — 1 if url starts with scheme://
 *
 * Minimal absolute form: ALPHA scheme-chars* "://"
 * Examples: "http://x" → 1, "HTTPS://x" → 1, "://x" → 0,
 * "/rel" → 0, "file:/x" → 0 (needs ://).
 */
int
gj_url_is_abs(const char *szUrl)
{
	size_t i;

	if (szUrl == NULL || szUrl[0] == '\0') {
		return 0;
	}

	if (!b795_is_alpha((unsigned char)szUrl[0])) {
		return 0;
	}

	i = 1u;
	while (szUrl[i] != '\0' && b795_is_scheme_ch((unsigned char)szUrl[i])) {
		i++;
	}

	if (szUrl[i] == ':' && szUrl[i + 1u] == '/' && szUrl[i + 2u] == '/') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_url_is_abs(const char *szUrl)
    __attribute__((alias("gj_url_is_abs")));
