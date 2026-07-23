/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3633: last delimited token view (_u).
 *
 * Surface (unique symbols):
 *   const char *gj_str_split_last_u(const char *s, int delim,
 *                                   size_t *out_len);
 *     - Return a pointer to the last delim-separated token of the
 *       NUL-terminated string s (octets after the final
 *       (unsigned char)delim, or the whole s when no delim is present).
 *       When out_len is non-NULL, write the token length (until NUL)
 *       into *out_len. s == NULL → NULL (out_len left untouched).
 *   const char *__gj_str_split_last_u  (alias)
 *   __libcgj_batch3633_marker = "libcgj-batch3633"
 *
 * Exclusive continuum CREATE-ONLY (3631-3640). Distinct from
 * gj_str_split_first / gj_str_split_first_u — unique last-token
 * surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3633_marker[] = "libcgj-batch3633";

/* ---- freestanding helpers ---------------------------------------------- */

static const char *
b3633_split_last(const char *s, int nDelim, size_t *pcbOut)
{
	size_t i;
	size_t iLast;
	int fFound;
	unsigned char uDelim;
	const char *pTok;
	size_t nTok;

	if (s == NULL) {
		return NULL;
	}

	uDelim = (unsigned char)nDelim;
	fFound = 0;
	iLast = 0u;
	i = 0u;
	while (s[i] != '\0') {
		if ((unsigned char)s[i] == uDelim) {
			fFound = 1;
			iLast = i;
		}
		i++;
	}

	if (fFound != 0) {
		pTok = s + iLast + 1u;
	} else {
		pTok = s;
	}

	nTok = 0u;
	while (pTok[nTok] != '\0') {
		nTok++;
	}

	if (pcbOut != NULL) {
		*pcbOut = nTok;
	}
	return pTok;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_split_last_u - pointer + length of the last delim-separated token.
 *
 * s:       NUL-terminated subject, or NULL
 * delim:   separator byte (compared as unsigned char)
 * out_len: optional destination for token length (may be NULL)
 *
 * Returns a pointer into s at the start of the last token, or NULL when
 * s is NULL. If delim never occurs, the whole string is the last token.
 * Examples: "a:b:c" / ':' → "c"; "solo" / ':' → "solo"; ":x" / ':' → "x".
 * No parent wires.
 */
const char *
gj_str_split_last_u(const char *s, int delim, size_t *out_len)
{
	(void)NULL;
	(void)(uint32_t)0;
	return b3633_split_last(s, delim, out_len);
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_str_split_last_u(const char *s, int delim, size_t *out_len)
    __attribute__((alias("gj_str_split_last_u")));
