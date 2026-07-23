/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1128: Content-Type charset parameter probe.
 *
 * Surface (unique symbols):
 *   int gj_content_type_has_charset(const char *ct);
 *     — Return 1 if ct contains a case-insensitive "charset="
 *       parameter after a ';' (OWS allowed), or as a leading
 *       "charset=" token. NULL / empty → 0. Scan capped at 1024.
 *   int __gj_content_type_has_charset  (alias)
 *   __libcgj_batch1128_marker = "libcgj-batch1128"
 *
 * Distinct from gj_mime_type_ok — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1128_marker[] = "libcgj-batch1128";

/* ---- helpers ------------------------------------------------------------ */

static unsigned char
b1128_lower(unsigned char uCh)
{
	if (uCh >= 'A' && uCh <= 'Z') {
		return (unsigned char)(uCh + ('a' - 'A'));
	}
	return uCh;
}

/*
 * Match "charset=" case-insensitively at sz; returns 1 on match.
 */
static int
b1128_is_charset_eq(const char *sz)
{
	static const char s_szKey[] = "charset=";
	size_t i;

	for (i = 0u; s_szKey[i] != '\0'; i++) {
		if (sz[i] == '\0') {
			return 0;
		}
		if (b1128_lower((unsigned char)sz[i]) !=
		    (unsigned char)s_szKey[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_content_type_has_charset — 1 if a charset= parameter is present.
 */
int
gj_content_type_has_charset(const char *szCt)
{
	size_t i;
	size_t j;
	size_t cb;

	if (szCt == NULL) {
		return 0;
	}

	cb = 0u;
	while (szCt[cb] != '\0') {
		cb++;
		if (cb > 1024u) {
			return 0;
		}
	}
	if (cb == 0u) {
		return 0;
	}

	/* Leading "charset=" (unusual but accept). */
	if (b1128_is_charset_eq(szCt)) {
		return 1;
	}

	/* After each ';' skip OWS then look for charset=. */
	for (i = 0u; i < cb; i++) {
		if (szCt[i] != ';') {
			continue;
		}
		j = i + 1u;
		while (j < cb && (szCt[j] == ' ' || szCt[j] == '\t')) {
			j++;
		}
		if (j < cb && b1128_is_charset_eq(szCt + j)) {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_content_type_has_charset(const char *szCt)
    __attribute__((alias("gj_content_type_has_charset")));
