/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3365: TOML standard table header predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_toml_is_table_u(const char *s, size_t n);
 *     - Return 1 if s[0..n) starts a TOML standard table header:
 *       first non-horizontal-ws is '[', the next octet is not '[',
 *       and a matching ']' appears before CR/LF or end. Else 0.
 *       Distinguishes from array-of-tables '[[...]]'. NULL / empty → 0.
 *       Does not validate table-name body chars.
 *   int __gj_toml_is_table_u  (alias)
 *   __libcgj_batch3365_marker = "libcgj-batch3365"
 *
 * Exclusive continuum CREATE-ONLY (3361-3370). Distinct from
 * gj_toml_is_comment_u (batch2988) / gj_ini_is_section_u (batch2989) —
 * TOML standard table form; unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3365_marker[] = "libcgj-batch3365";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is horizontal space or tab. */
static int
b3365_is_hws(unsigned char uCh)
{
	return (uCh == (unsigned char)' ' || uCh == (unsigned char)'\t')
	    ? 1 : 0;
}

/* 1 if uCh ends a logical line. */
static int
b3365_is_eol(unsigned char uCh)
{
	return (uCh == (unsigned char)'\n' || uCh == (unsigned char)'\r')
	    ? 1 : 0;
}

/*
 * 1 if first non-hws is '[', second is not '[', and ']' before EOL/end.
 */
static int
b3365_is_table(const char *sz, size_t cN)
{
	size_t i;
	int fOpen;

	fOpen = 0;
	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (fOpen == 0) {
			if (b3365_is_hws(uCh) != 0) {
				continue;
			}
			if (uCh != (unsigned char)'[') {
				return 0;
			}
			/* Reject array-of-tables '[['. */
			if ((i + 1u) < cN &&
			    (unsigned char)sz[i + 1u] == (unsigned char)'[') {
				return 0;
			}
			fOpen = 1;
			continue;
		}
		/* inside [ ... ] */
		if (b3365_is_eol(uCh) != 0) {
			return 0;
		}
		if (uCh == (unsigned char)']') {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_toml_is_table_u - 1 if s starts a TOML standard [table] header.
 *
 * Leading spaces/tabs allowed; rejects '[[array.table]]' form.
 */
int
gj_toml_is_table_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b3365_is_table(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_toml_is_table_u(const char *sz, size_t cN)
    __attribute__((alias("gj_toml_is_table_u")));
