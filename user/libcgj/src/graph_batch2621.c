/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2621: freestanding C-string length (_u wave).
 *
 * Surface (unique symbols):
 *   size_t gj_str_len_u(const char *s);
 *     - Count octets in s before the first NUL. s == NULL → 0.
 *   size_t __gj_str_len_u  (alias)
 *   __libcgj_batch2621_marker = "libcgj-batch2621"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_len (batch411) and gj_str_nlen_u (batch2204) — unique _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2621_marker[] = "libcgj-batch2621";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2621_len(const char *sz)
{
	size_t n;

	if (sz == NULL) {
		return 0u;
	}

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_len_u - length of a NUL-terminated string (not counting the NUL).
 *
 * s: NUL-terminated string, or NULL
 *
 * Does not call libc strlen. Returns 0 when s is NULL.
 */
size_t
gj_str_len_u(const char *s)
{
	(void)NULL;
	return b2621_len(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_len_u(const char *s)
    __attribute__((alias("gj_str_len_u")));
