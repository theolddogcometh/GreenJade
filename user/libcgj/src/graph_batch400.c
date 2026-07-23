/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch400: freestanding memory character search.
 *
 * Surface (unique symbols):
 *   const void *gj_mem_chr(const void *s, int c, size_t n);
 *     — Search the first n octets of s for the first occurrence of
 *       (unsigned char)c. Returns a pointer to that octet, or NULL if
 *       not found. s == NULL or n == 0 → NULL.
 *   const void *__gj_mem_chr  (alias)
 *   __libcgj_batch400_marker = "libcgj-batch400"
 *
 * Distinct from libc memchr in string.c — unique gj_ names only; return
 * type is const void * (read-only view of the input window).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch400_marker[] = "libcgj-batch400";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_chr — find first octet equal to (unsigned char)c in s[0..n).
 *
 * s: region of at least n readable bytes when n > 0 (NULL → NULL)
 * c: search byte (truncated to unsigned char)
 * n: number of octets to scan (0 → NULL)
 *
 * Does not call libc memchr. Returns const view into s, or NULL.
 */
const void *
gj_mem_chr(const void *s, int c, size_t n)
{
	const unsigned char *ps;
	unsigned char ch;
	size_t i;

	if (s == NULL || n == 0u) {
		return NULL;
	}

	ps = (const unsigned char *)s;
	ch = (unsigned char)c;
	for (i = 0u; i < n; i++) {
		if (ps[i] == ch) {
			return (const void *)(ps + i);
		}
	}
	return NULL;
}

/* ---- underscored alias ------------------------------------------------- */

const void *__gj_mem_chr(const void *s, int c, size_t n)
    __attribute__((alias("gj_mem_chr")));
