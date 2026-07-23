/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2989: INI [section] line start predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ini_is_section_u(const char *s, size_t n);
 *     - Return 1 if s[0..n) starts an INI section header: first
 *       non-horizontal-ws octet is '[', and a matching ']' appears
 *       later on the same logical line (before CR/LF or end). Else 0.
 *       NULL / empty → 0. Does not validate section-name body chars.
 *   int __gj_ini_is_section_u  (alias)
 *   __libcgj_batch2989_marker = "libcgj-batch2989"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_ini_is_section_char (batch1598 char predicate) — full line form;
 * unique name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2989_marker[] = "libcgj-batch2989";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uCh is horizontal space or tab. */
static int
b2989_is_hws(unsigned char uCh)
{
	return (uCh == (unsigned char)' ' || uCh == (unsigned char)'\t')
	    ? 1 : 0;
}

/* 1 if uCh ends a logical line. */
static int
b2989_is_eol(unsigned char uCh)
{
	return (uCh == (unsigned char)'\n' || uCh == (unsigned char)'\r')
	    ? 1 : 0;
}

/*
 * 1 if first non-hws is '[' and a ']' appears before EOL/end.
 */
static int
b2989_is_section(const char *sz, size_t cN)
{
	size_t i;
	int fOpen;

	fOpen = 0;
	for (i = 0u; i < cN; i++) {
		unsigned char uCh = (unsigned char)sz[i];

		if (fOpen == 0) {
			if (b2989_is_hws(uCh) != 0) {
				continue;
			}
			if (uCh != (unsigned char)'[') {
				return 0;
			}
			fOpen = 1;
			continue;
		}
		/* inside [ ... ] */
		if (b2989_is_eol(uCh) != 0) {
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
 * gj_ini_is_section_u - 1 if s starts an INI [section] header line.
 *
 * Leading spaces/tabs allowed; requires '[' ... ']' before CR/LF.
 */
int
gj_ini_is_section_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL || cN == 0u) {
		return 0;
	}
	return b2989_is_section(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ini_is_section_u(const char *sz, size_t cN)
    __attribute__((alias("gj_ini_is_section_u")));
