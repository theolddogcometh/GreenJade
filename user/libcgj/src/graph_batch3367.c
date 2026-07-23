/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3367: INI key=value line predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ini_is_keyval_u(const char *s, size_t n);
 *     - Return 1 if s[0..n) looks like an INI key=value assignment line:
 *       after optional leading spaces/tabs, first significant octet is
 *       not '#' / ';' / '[' / CR / LF, there is at least one key body
 *       octet before '=', and '=' appears before CR/LF or end. Else 0.
 *       NULL / empty → 0. Does not validate value body.
 *   int __gj_ini_is_keyval_u  (alias)
 *   __libcgj_batch3367_marker = "libcgj-batch3367"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_ini_is_section_u (batch2989) / gj_ini_is_comment_char (batch914) —
 * key=value line form; unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3367_marker[] = "libcgj-batch3367";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is horizontal space or tab. */
static int
b3367_is_hws(unsigned char uCh)
{
	return (uCh == (unsigned char)' ' || uCh == (unsigned char)'\t')
	    ? 1 : 0;
}

/* 1 if uCh ends a logical line. */
static int
b3367_is_eol(unsigned char uCh)
{
	return (uCh == (unsigned char)'\n' || uCh == (unsigned char)'\r')
	    ? 1 : 0;
}

/*
 * 1 if line is key=value (not section/comment/blank).
 * Key body may include spaces before '='; at least one non-hws key char
 * is required before '='.
 */
static int
b3367_is_keyval(const char *sz, size_t cN)
{
	size_t i;
	int fKey;

	fKey = 0;
	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (fKey == 0) {
			if (b3367_is_hws(uCh) != 0) {
				continue;
			}
			/* Reject comments, sections, bare EOL. */
			if (uCh == (unsigned char)'#' ||
			    uCh == (unsigned char)';' ||
			    uCh == (unsigned char)'[' ||
			    b3367_is_eol(uCh) != 0) {
				return 0;
			}
			if (uCh == (unsigned char)'=') {
				/* Empty key not accepted. */
				return 0;
			}
			fKey = 1;
			continue;
		}
		/* In key body: need '=' before EOL. */
		if (b3367_is_eol(uCh) != 0) {
			return 0;
		}
		if (uCh == (unsigned char)'=') {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ini_is_keyval_u - 1 if s starts an INI key=value assignment line.
 *
 * Leading spaces/tabs allowed; rejects section/comment/blank lines.
 */
int
gj_ini_is_keyval_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b3367_is_keyval(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ini_is_keyval_u(const char *sz, size_t cN)
    __attribute__((alias("gj_ini_is_keyval_u")));
