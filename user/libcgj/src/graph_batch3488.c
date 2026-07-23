/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3488: MIME type is-text predicate (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_mime_is_text_u(const char *type);
 *     - Return 1u if type begins with the case-insensitive prefix
 *       "text/" and has at least one non-empty subtype character after
 *       the slash. ASCII A-Z is folded to a-z for the type token only.
 *       NULL, empty, "text", "text/", or non-text types -> 0u.
 *   unsigned __gj_mime_is_text_u  (alias)
 *   __libcgj_batch3488_marker = "libcgj-batch3488"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Distinct from
 * gj_mime_type_ok (batch1127) and gj_mime_type_has_slash (batch1957).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3488_marker[] = "libcgj-batch3488";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned char
b3488_fold(unsigned char uCh)
{
	if (uCh >= (unsigned char)'A' && uCh <= (unsigned char)'Z') {
		return (unsigned char)(uCh + (unsigned char)('a' - 'A'));
	}
	return uCh;
}

/* Match ASCII case-insensitive literal "text/" then a non-empty subtype. */
static unsigned
b3488_is_text(const char *szType)
{
	static const char s_pfx[] = "text/";
	size_t i;

	if (szType == NULL || szType[0] == '\0') {
		return 0u;
	}

	for (i = 0u; s_pfx[i] != '\0'; i++) {
		if (szType[i] == '\0') {
			return 0u;
		}
		if (b3488_fold((unsigned char)szType[i]) !=
		    (unsigned char)s_pfx[i]) {
			return 0u;
		}
	}

	/* Require at least one subtype character. */
	if (szType[i] == '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mime_is_text_u - 1u if type looks like text/<subtype>.
 *
 * Examples:
 *   "text/plain"   -> 1u
 *   "TEXT/HTML"    -> 1u
 *   "text/css"     -> 1u
 *   "text/"        -> 0u
 *   "text"         -> 0u
 *   "image/png"    -> 0u
 *   NULL / ""      -> 0u
 *
 * Does not call libc. No parent wires.
 */
unsigned
gj_mime_is_text_u(const char *szType)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3488_is_text(szType);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_mime_is_text_u(const char *szType)
    __attribute__((alias("gj_mime_is_text_u")));
