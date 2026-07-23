/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch364: find character or return end-of-string.
 *
 * Surface (unique symbols):
 *   char *gj_strchr_nul(const char *s, int c);
 *     — Search s for the first occurrence of (unsigned char)c. If found,
 *       return a pointer to that octet. If not found, return a pointer
 *       to the terminating NUL of s (GNU strchrnul-shaped). c may be 0,
 *       in which case the returned pointer is the string's end. NULL s
 *       returns NULL.
 *   char *__gj_strchr_nul  (alias)
 *   __libcgj_batch364_marker = "libcgj-batch364"
 *
 * Distinct from libc strchr / GNU strchrnul — unique gj_ names only;
 * no multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch364_marker[] = "libcgj-batch364";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strchr_nul — locate c in s, or the terminating NUL if absent.
 *
 * s: NUL-terminated subject (NULL → NULL)
 * c: character to find (compared as unsigned char)
 *
 * Returns a non-const char * into s (cast through the const parameter),
 * matching common strchr / strchrnul calling conventions. Does not call
 * libc strchr or strchrnul.
 */
char *
gj_strchr_nul(const char *s, int c)
{
	unsigned char u8Want;
	const char *p;

	if (s == NULL) {
		return NULL;
	}

	u8Want = (unsigned char)c;
	p = s;
	for (;;) {
		if ((unsigned char)*p == u8Want) {
			return (char *)(void *)p;
		}
		if (*p == '\0') {
			/* Not found: return pointer to the terminating NUL. */
			return (char *)(void *)p;
		}
		p++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_strchr_nul(const char *s, int c)
    __attribute__((alias("gj_strchr_nul")));
