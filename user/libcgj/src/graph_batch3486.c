/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3486: URI scheme length (_u form).
 *
 * Surface (unique symbols):
 *   size_t gj_uri_scheme_len_u(const char *uri);
 *     - Length of a leading RFC 3986 scheme (ALPHA *(ALPHA/DIGIT/"+/-/."))
 *       when followed by ':'. Returns 0 if no valid scheme prefix.
 *       Does not include the ':'. Distinct from gj_uri_scheme_len
 *       (batch1416) by the _u suffix surface name only.
 *   size_t __gj_uri_scheme_len_u  (alias)
 *   __libcgj_batch3486_marker = "libcgj-batch3486"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Unique
 * gj_uri_scheme_len_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3486_marker[] = "libcgj-batch3486";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3486_is_alpha(unsigned char uCh)
{
	return (uCh >= (unsigned char)'A' && uCh <= (unsigned char)'Z') ||
	       (uCh >= (unsigned char)'a' && uCh <= (unsigned char)'z');
}

static int
b3486_is_scheme_cont(unsigned char uCh)
{
	return b3486_is_alpha(uCh) ||
	       (uCh >= (unsigned char)'0' && uCh <= (unsigned char)'9') ||
	       uCh == (unsigned char)'+' ||
	       uCh == (unsigned char)'-' ||
	       uCh == (unsigned char)'.';
}

static size_t
b3486_scheme_len(const char *szUri)
{
	size_t i;

	if (szUri == NULL || szUri[0] == '\0') {
		return 0u;
	}

	if (!b3486_is_alpha((unsigned char)szUri[0])) {
		return 0u;
	}

	i = 1u;
	while (szUri[i] != '\0' &&
	    b3486_is_scheme_cont((unsigned char)szUri[i])) {
		i++;
	}

	if (szUri[i] != ':') {
		return 0u;
	}

	return i;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_scheme_len_u - length of scheme before ':', or 0.
 *
 * Examples:
 *   "http://x"  -> 4
 *   "HTTPS://x" -> 5
 *   "mailto:a"  -> 6
 *   "a+b.c:d"   -> 5
 *   "1http://x" -> 0  (scheme must start with ALPHA)
 *   "http"      -> 0  (no ':')
 *   NULL / ""   -> 0
 *
 * Does not call libc. No parent wires.
 */
size_t
gj_uri_scheme_len_u(const char *szUri)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3486_scheme_len(szUri);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_uri_scheme_len_u(const char *szUri)
    __attribute__((alias("gj_uri_scheme_len_u")));
