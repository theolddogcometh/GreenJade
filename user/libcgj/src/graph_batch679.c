/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch679: first delimited token view (read-only).
 *
 * Surface (unique symbols):
 *   const char *gj_str_split_first(const char *s, int delim, size_t *out_len);
 *     — Return a pointer to the first token of the NUL-terminated string s
 *       (the start of s). When out_len is non-NULL, write the token length
 *       (octets until the first (unsigned char)delim or until the
 *       terminating NUL, exclusive of both) into *out_len. s == NULL →
 *       NULL (out_len left untouched).
 *   const char *__gj_str_split_first  (alias)
 *   __libcgj_batch679_marker = "libcgj-batch679"
 *
 * Does not allocate, mutate s, or call libc strtok / strsep. Unique gj_
 * names only — no multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch679_marker[] = "libcgj-batch679";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_split_first — pointer + length of the first delim-separated token.
 *
 * s:       NUL-terminated subject, or NULL
 * delim:   separator byte (compared as unsigned char)
 * out_len: optional destination for token length (may be NULL)
 *
 * Returns s (start of the first token) on success, or NULL when s is
 * NULL. The token ends at the first matching delim or at the string
 * end; the delimiter itself is not part of the reported length.
 */
const char *
gj_str_split_first(const char *s, int delim, size_t *out_len)
{
	size_t n;
	unsigned char d;

	if (s == NULL) {
		return NULL;
	}

	d = (unsigned char)delim;
	n = 0u;
	while (s[n] != '\0' && (unsigned char)s[n] != d) {
		n++;
	}

	if (out_len != NULL) {
		*out_len = n;
	}

	return s;
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_str_split_first(const char *s, int delim, size_t *out_len)
    __attribute__((alias("gj_str_split_first")));
