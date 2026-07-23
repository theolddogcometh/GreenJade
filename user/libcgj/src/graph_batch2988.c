/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2988: TOML full-line comment predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_toml_is_comment_u(const char *s, size_t n);
 *     - Return 1 if the first non-horizontal-whitespace octet in
 *       s[0..n) is '#' (TOML comment introducer). Horizontal ws is
 *       space and tab only (not CR/LF). Else 0. NULL / empty /
 *       all-ws without '#' → 0.
 *   int __gj_toml_is_comment_u  (alias)
 *   __libcgj_batch2988_marker = "libcgj-batch2988"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_ini_is_comment_char (batch914) — TOML '#' only, buffer form;
 * unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2988_marker[] = "libcgj-batch2988";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is horizontal TOML/ASCII space or tab. */
static int
b2988_is_hws(unsigned char uCh)
{
	return (uCh == (unsigned char)' ' || uCh == (unsigned char)'\t')
	    ? 1 : 0;
}

/* 1 if first non-hws octet is '#'. */
static int
b2988_is_comment(const char *sz, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (b2988_is_hws(uCh) != 0) {
			continue;
		}
		return (uCh == (unsigned char)'#') ? 1 : 0;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_is_comment_u - 1 if s is a TOML comment line start (#).
 *
 * Leading spaces/tabs allowed; first significant char must be '#'.
 */
int
gj_toml_is_comment_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b2988_is_comment(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_is_comment_u(const char *sz, size_t cN)
    __attribute__((alias("gj_toml_is_comment_u")));
