/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3366: TOML array-of-tables header predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_toml_is_array_table_u(const char *s, size_t n);
 *     - Return 1 if s[0..n) starts a TOML array-of-tables header:
 *       first non-horizontal-ws octets are "[[", and a matching "]]"
 *       pair appears later on the same logical line (before CR/LF or
 *       end) — specifically a second ']' after an earlier ']'. Else 0.
 *       NULL / empty → 0. Does not validate name body chars.
 *   int __gj_toml_is_array_table_u  (alias)
 *   __libcgj_batch3366_marker = "libcgj-batch3366"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_toml_is_table_u (batch3365 standard [table]) — double-bracket
 * form; unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3366_marker[] = "libcgj-batch3366";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is horizontal space or tab. */
static int
b3366_is_hws(unsigned char uCh)
{
	return (uCh == (unsigned char)' ' || uCh == (unsigned char)'\t')
	    ? 1 : 0;
}

/* 1 if uCh ends a logical line. */
static int
b3366_is_eol(unsigned char uCh)
{
	return (uCh == (unsigned char)'\n' || uCh == (unsigned char)'\r')
	    ? 1 : 0;
}

/*
 * 1 if first non-hws is "[[" and two ']' appear before EOL/end.
 */
static int
b3366_is_array_table(const char *sz, size_t cN)
{
	size_t i;
	size_t cClose;
	int fOpen;

	fOpen = 0;
	cClose = 0u;
	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (fOpen == 0) {
			if (b3366_is_hws(uCh) != 0) {
				continue;
			}
			/* Require "[[" at first significant position. */
			if (uCh != (unsigned char)'[') {
				return 0;
			}
			if ((i + 1u) >= cN ||
			    (unsigned char)sz[i + 1u] != (unsigned char)'[') {
				return 0;
			}
			fOpen = 1;
			i++; /* consume the second '[' of "[[" */
			continue;
		}
		if (b3366_is_eol(uCh) != 0) {
			return 0;
		}
		if (uCh == (unsigned char)']') {
			cClose++;
			if (cClose >= 2u) {
				return 1;
			}
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_is_array_table_u - 1 if s starts a TOML [[array.table]] header.
 *
 * Leading spaces/tabs allowed; requires "[[" ... "]]" before CR/LF.
 */
int
gj_toml_is_array_table_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b3366_is_array_table(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_is_array_table_u(const char *sz, size_t cN)
    __attribute__((alias("gj_toml_is_array_table_u")));
