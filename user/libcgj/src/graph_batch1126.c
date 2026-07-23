/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1126: URI userinfo validation (RFC 3986).
 *
 * Surface (unique symbols):
 *   int gj_uri_userinfo_ok(const char *userinfo);
 *     — Return 1 if userinfo is zero or more of:
 *       unreserved / pct-encoded / sub-delims / ":"
 *       unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
 *       sub-delims = "!" / "$" / "&" / "'" / "(" / ")" /
 *                    "*" / "+" / "," / ";" / "="
 *       pct-encoded = "%" HEXDIG HEXDIG
 *       Empty string is valid. NULL → 0. Max length 1024.
 *   int __gj_uri_userinfo_ok  (alias)
 *   __libcgj_batch1126_marker = "libcgj-batch1126"
 *
 * Distinct from gj_uri_scheme_ok — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1126_marker[] = "libcgj-batch1126";

/* ---- helpers ------------------------------------------------------------ */

static int
b1126_is_hex(unsigned char uCh)
{
	return ((uCh >= '0' && uCh <= '9') ||
		(uCh >= 'A' && uCh <= 'F') ||
		(uCh >= 'a' && uCh <= 'f')) ? 1 : 0;
}

static int
b1126_is_unreserved(unsigned char uCh)
{
	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1;
	}
	return (uCh == '-' || uCh == '.' || uCh == '_' || uCh == '~') ? 1 : 0;
}

static int
b1126_is_subdelim(unsigned char uCh)
{
	switch (uCh) {
	case '!':
	case '$':
	case '&':
	case '\'':
	case '(':
	case ')':
	case '*':
	case '+':
	case ',':
	case ';':
	case '=':
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_userinfo_ok — 1 if szUserinfo is valid URI userinfo.
 */
int
gj_uri_userinfo_ok(const char *szUserinfo)
{
	size_t i;
	unsigned char uCh;

	if (szUserinfo == NULL) {
		return 0;
	}

	i = 0u;
	while (szUserinfo[i] != '\0') {
		if (i >= 1024u) {
			return 0;
		}
		uCh = (unsigned char)szUserinfo[i];
		if (uCh == '%') {
			if (szUserinfo[i + 1u] == '\0' ||
			    szUserinfo[i + 2u] == '\0') {
				return 0;
			}
			if (!b1126_is_hex((unsigned char)szUserinfo[i + 1u]) ||
			    !b1126_is_hex((unsigned char)szUserinfo[i + 2u])) {
				return 0;
			}
			i += 3u;
			continue;
		}
		if (b1126_is_unreserved(uCh) || b1126_is_subdelim(uCh) ||
		    uCh == ':') {
			i++;
			continue;
		}
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uri_userinfo_ok(const char *szUserinfo)
    __attribute__((alias("gj_uri_userinfo_ok")));
