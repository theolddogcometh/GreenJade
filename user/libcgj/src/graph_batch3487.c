/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3487: URI has-scheme predicate (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_uri_has_scheme_u(const char *uri);
 *     - Return 1u if uri has a leading RFC 3986 scheme followed by ':',
 *       else 0u. Same acceptance rules as gj_uri_scheme_len_u: first
 *       character ALPHA, then ALPHA/DIGIT/"+/-/.", then ':'.
 *       NULL or empty -> 0u.
 *   unsigned __gj_uri_has_scheme_u  (alias)
 *   __libcgj_batch3487_marker = "libcgj-batch3487"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Distinct from
 * gj_uri_scheme_ok (batch1125 bare-scheme validate), gj_uri_scheme_len
 * (batch1416), and gj_uri_scheme_len_u (batch3486). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3487_marker[] = "libcgj-batch3487";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3487_is_alpha(unsigned char uCh)
{
	return (uCh >= (unsigned char)'A' && uCh <= (unsigned char)'Z') ||
	       (uCh >= (unsigned char)'a' && uCh <= (unsigned char)'z');
}

static int
b3487_is_scheme_cont(unsigned char uCh)
{
	return b3487_is_alpha(uCh) ||
	       (uCh >= (unsigned char)'0' && uCh <= (unsigned char)'9') ||
	       uCh == (unsigned char)'+' ||
	       uCh == (unsigned char)'-' ||
	       uCh == (unsigned char)'.';
}

static unsigned
b3487_has_scheme(const char *szUri)
{
	size_t i;

	if (szUri == NULL || szUri[0] == '\0') {
		return 0u;
	}

	if (!b3487_is_alpha((unsigned char)szUri[0])) {
		return 0u;
	}

	i = 1u;
	while (szUri[i] != '\0' &&
	    b3487_is_scheme_cont((unsigned char)szUri[i])) {
		i++;
	}

	if (szUri[i] == ':') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_has_scheme_u - 1u if uri begins with a valid scheme':'.
 *
 * Examples:
 *   "http://x" -> 1u
 *   "mailto:a" -> 1u
 *   "http"     -> 0u
 *   "1ab:c"    -> 0u
 *   NULL / ""  -> 0u
 *
 * Self-contained (no call into batch3486). Does not call libc.
 * No parent wires.
 */
unsigned
gj_uri_has_scheme_u(const char *szUri)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3487_has_scheme(szUri);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uri_has_scheme_u(const char *szUri)
    __attribute__((alias("gj_uri_has_scheme_u")));
