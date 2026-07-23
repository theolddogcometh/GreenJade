/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1416: URI scheme length (before ':').
 *
 * Surface (unique symbols):
 *   size_t gj_uri_scheme_len(const char *uri);
 *     — Length of a leading RFC 3986 scheme (ALPHA *(ALPHA/DIGIT/"+/-/."))
 *       when followed by ':'. Returns 0 if no valid scheme prefix.
 *       Does not include the ':'.
 *   size_t __gj_uri_scheme_len  (alias)
 *   __libcgj_batch1416_marker = "libcgj-batch1416"
 *
 * Distinct from gj_uri_scheme_ok (batch1125) which validates a bare scheme
 * string — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1416_marker[] = "libcgj-batch1416";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1416_is_alpha(unsigned char uCh)
{
	return (uCh >= 'A' && uCh <= 'Z') || (uCh >= 'a' && uCh <= 'z');
}

static int
b1416_is_scheme_cont(unsigned char uCh)
{
	return b1416_is_alpha(uCh) ||
	       (uCh >= '0' && uCh <= '9') ||
	       uCh == '+' || uCh == '-' || uCh == '.';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_scheme_len — length of scheme before ':', or 0.
 *
 * Examples:
 *   "http://x"  → 4
 *   "HTTPS://x" → 5
 *   "mailto:a"  → 6
 *   "a+b.c:d"   → 5
 *   "1http://x" → 0  (scheme must start with ALPHA)
 *   "http"      → 0  (no ':')
 *   ":foo"      → 0
 *   NULL / ""   → 0
 */
size_t
gj_uri_scheme_len(const char *szUri)
{
	size_t i;

	if (szUri == NULL || szUri[0] == '\0') {
		return 0u;
	}

	if (!b1416_is_alpha((unsigned char)szUri[0])) {
		return 0u;
	}

	i = 1u;
	while (szUri[i] != '\0' && b1416_is_scheme_cont((unsigned char)szUri[i])) {
		i++;
	}

	if (szUri[i] != ':') {
		return 0u;
	}

	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_uri_scheme_len(const char *szUri)
    __attribute__((alias("gj_uri_scheme_len")));
