/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3632: first delimited token view (_u).
 *
 * Surface (unique symbols):
 *   const char *gj_str_split_first_u(const char *s, int delim,
 *                                    size_t *out_len);
 *     - Return a pointer to the first token of the NUL-terminated string
 *       s (the start of s). When out_len is non-NULL, write the token
 *       length (octets until the first (unsigned char)delim or until the
 *       terminating NUL, exclusive of both) into *out_len. s == NULL →
 *       NULL (out_len left untouched).
 *   const char *__gj_str_split_first_u  (alias)
 *   __libcgj_batch3632_marker = "libcgj-batch3632"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_split_first (batch679) — unique _u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3632_marker[] = "libcgj-batch3632";

/* ---- freestanding helpers ---------------------------------------------- */

static const char *
b3632_split_first(const char *s, int nDelim, size_t *pcbOut)
{
	size_t n;
	unsigned char uDelim;

	if (s == NULL) {
		return NULL;
	}

	uDelim = (unsigned char)nDelim;
	n = 0u;
	while (s[n] != '\0' && (unsigned char)s[n] != uDelim) {
		n++;
	}

	if (pcbOut != NULL) {
		*pcbOut = n;
	}
	return s;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_split_first_u - pointer + length of the first delim-separated token.
 *
 * s:       NUL-terminated subject, or NULL
 * delim:   separator byte (compared as unsigned char)
 * out_len: optional destination for token length (may be NULL)
 *
 * Returns s (start of the first token) on success, or NULL when s is
 * NULL. The token ends at the first matching delim or at the string
 * end; the delimiter itself is not part of the reported length.
 * No parent wires.
 */
const char *
gj_str_split_first_u(const char *s, int delim, size_t *out_len)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3632_split_first(s, delim, out_len);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_str_split_first_u(const char *s, int delim, size_t *out_len)
    __attribute__((alias("gj_str_split_first_u")));
