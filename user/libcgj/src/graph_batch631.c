/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch631: freestanding bounded C-string length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_nlen(const char *s, size_t maxlen);
 *     — Count octets in s before the first NUL, stopping at maxlen.
 *       s == NULL → 0. Never reads past s[maxlen-1].
 *   size_t __gj_str_nlen  (alias)
 *   __libcgj_batch631_marker = "libcgj-batch631"
 *
 * Like POSIX strnlen. Distinct from libc strnlen in string.c and from
 * gj_str_len (batch411) — unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch631_marker[] = "libcgj-batch631";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_nlen — bounded length of a NUL-terminated string.
 *
 * s:      NUL-terminated string, or NULL
 * maxlen: maximum number of octets to examine
 *
 * Returns the number of octets before the first NUL, or maxlen if no
 * NUL appears in the first maxlen octets. Returns 0 when s is NULL.
 * Does not call libc strnlen/strlen.
 */
size_t
gj_str_nlen(const char *s, size_t maxlen)
{
	size_t n;

	if (s == NULL) {
		return 0u;
	}

	n = 0u;
	while (n < maxlen && s[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_nlen(const char *s, size_t maxlen)
    __attribute__((alias("gj_str_nlen")));
