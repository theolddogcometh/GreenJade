/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2204: freestanding bounded C-string length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_nlen_u(const char *s, size_t maxlen);
 *     - Count octets in s before the first NUL, stopping at maxlen.
 *       s == NULL → 0. Never reads past s[maxlen-1].
 *   size_t __gj_str_nlen_u  (alias)
 *   __libcgj_batch2204_marker = "libcgj-batch2204"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_nlen
 * (batch631) and libc strnlen - unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2204_marker[] = "libcgj-batch2204";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2204_nlen(const char *sz, size_t cMax)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}

	n = 0u;
	while (n < cMax && sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_nlen_u - bounded length of a NUL-terminated string.
 *
 * s:      NUL-terminated string, or NULL
 * maxlen: maximum number of octets to examine
 *
 * Returns the number of octets before the first NUL, or maxlen if no
 * NUL appears in the first maxlen octets. Returns 0 when s is NULL.
 * Does not call libc strnlen/strlen.
 */
size_t
gj_str_nlen_u(const char *s, size_t maxlen)
{
	(void)NULL;
	return b2204_nlen(s, maxlen);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_nlen_u(const char *s, size_t maxlen)
    __attribute__((alias("gj_str_nlen_u")));
