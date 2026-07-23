/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch415: freestanding C-string character search.
 *
 * Surface (unique symbols):
 *   char *gj_str_chr(const char *s, int c);
 *     — Return a pointer to the first occurrence of (unsigned char)c in
 *       the NUL-terminated string s, or NULL if not found. The
 *       terminating NUL is included in the search (c == 0 finds the end).
 *       s == NULL → NULL.
 *   char *__gj_str_chr  (alias)
 *   __libcgj_batch415_marker = "libcgj-batch415"
 *
 * Distinct from libc strchr in string.c — unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch415_marker[] = "libcgj-batch415";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_chr — find first occurrence of (unsigned char)c in s (incl. NUL).
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Does not call libc strchr. Returns pointer into s, or NULL.
 */
char *
gj_str_chr(const char *s, int c)
{
	unsigned char ch;
	const unsigned char *p;

	if (s == NULL) {
		return NULL;
	}

	ch = (unsigned char)c;
	p = (const unsigned char *)s;
	for (;;) {
		if (*p == ch) {
			return (char *)(uintptr_t)p;
		}
		if (*p == '\0') {
			return NULL;
		}
		p++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_chr(const char *s, int c)
    __attribute__((alias("gj_str_chr")));
