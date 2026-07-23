/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2626: freestanding C-string character search.
 *
 * Surface (unique symbols):
 *   char *gj_str_chr_u(const char *s, int c);
 *     - Return a pointer to the first occurrence of (unsigned char)c in
 *       the NUL-terminated string s, or NULL if not found. The
 *       terminating NUL is included in the search (c == 0 finds the end).
 *       s == NULL → NULL.
 *   char *__gj_str_chr_u  (alias)
 *   __libcgj_batch2626_marker = "libcgj-batch2626"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_chr (batch415) and gj_str_chr_n_u (batch2205) — unique _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2626_marker[] = "libcgj-batch2626";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2626_chr(const char *sz, int nC)
{
	unsigned char ch;
	const unsigned char *p;

	if (sz == NULL) {
		return NULL;
	}

	ch = (unsigned char)nC;
	p = (const unsigned char *)sz;
	for (;;) {
		if (*p == ch) {
			return (char *)(void *)p;
		}
		if (*p == '\0') {
			return NULL;
		}
		p++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_chr_u - find first occurrence of (unsigned char)c in s (incl. NUL).
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Does not call libc strchr. Returns pointer into s, or NULL.
 */
char *
gj_str_chr_u(const char *s, int c)
{
	(void)NULL;
	return b2626_chr(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_chr_u(const char *s, int c)
    __attribute__((alias("gj_str_chr_u")));
