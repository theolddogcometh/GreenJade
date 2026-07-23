/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2627: freestanding C-string reverse character search.
 *
 * Surface (unique symbols):
 *   char *gj_str_rchr_u(const char *s, int c);
 *     - Return a pointer to the last occurrence of (unsigned char)c in
 *       the NUL-terminated string s, or NULL if not found. The
 *       terminating NUL is included in the search (c == 0 finds the end).
 *       s == NULL → NULL.
 *   char *__gj_str_rchr_u  (alias)
 *   __libcgj_batch2627_marker = "libcgj-batch2627"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_rchr_n_u (batch2206) and gj_str_chr_u (batch2626) — unique _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2627_marker[] = "libcgj-batch2627";

/* ---- freestanding helpers ---------------------------------------------- */

/* Length of sz including the terminating NUL (always >= 1 for non-NULL). */
static size_t
b2627_len_incl(const char *sz)
{
	size_t n;

	n = 0u;
	for (;;) {
		n++;
		if (sz[n - 1u] == '\0') {
			return n;
		}
	}
}

static char *
b2627_rchr(const char *sz, int nC)
{
	size_t cb;
	size_t i;
	unsigned char ch;
	const unsigned char *p;

	if (sz == NULL) {
		return NULL;
	}

	ch = (unsigned char)nC;
	/* Scan inclusive of terminator so c == 0 hits the end. */
	cb = b2627_len_incl(sz);
	p = (const unsigned char *)sz;
	i = cb;
	while (i > 0u) {
		i--;
		if (p[i] == ch) {
			return (char *)(void *)(sz + i);
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rchr_u - find last occurrence of (unsigned char)c in s (incl. NUL).
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Does not call libc strrchr. Returns pointer into s, or NULL.
 */
char *
gj_str_rchr_u(const char *s, int c)
{
	(void)NULL;
	return b2627_rchr(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_rchr_u(const char *s, int c)
    __attribute__((alias("gj_str_rchr_u")));
