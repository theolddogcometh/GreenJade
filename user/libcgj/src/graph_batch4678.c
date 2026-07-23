/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4678: index of last non-whitespace octet
 * (string-measure wave).
 *
 * Surface (unique symbols):
 *   size_t gj_str_last_nonws_u(const char *s);
 *     - Return the 0-based index of the last octet that is not space /
 *       tab / CR / LF. Empty string, all-whitespace, or NULL → SIZE_MAX.
 *   size_t __gj_str_last_nonws_u  (alias)
 *   __libcgj_batch4678_marker = "libcgj-batch4678"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Unique
 * gj_str_last_nonws_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4678_marker[] = "libcgj-batch4678";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is space, tab, CR, or LF. */
static int
b4678_is_ws(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

static size_t
b4678_last_nonws(const char *sz)
{
	size_t i;
	size_t iLast;

	if (sz == NULL) {
		return SIZE_MAX;
	}

	iLast = SIZE_MAX;
	i = 0u;
	while (sz[i] != '\0') {
		if (!b4678_is_ws((unsigned char)sz[i])) {
			iLast = i;
		}
		i++;
	}
	return iLast;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_last_nonws_u - index of the last non-whitespace octet in s.
 *
 * s: NUL-terminated string, or NULL
 *
 * Returns the 0-based index of the last non space/tab/CR/LF octet.
 * Returns SIZE_MAX when s is NULL, empty, or all whitespace.
 * Does not call libc. No parent wires.
 */
size_t
gj_str_last_nonws_u(const char *s)
{
	(void)NULL;
	return b4678_last_nonws(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_last_nonws_u(const char *s)
    __attribute__((alias("gj_str_last_nonws_u")));
