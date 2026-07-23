/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2751: freestanding C-string length (post-2750).
 *
 * Surface (unique symbols):
 *   size_t gj_str_length_u(const char *s);
 *     - Count octets in s before the first NUL. s == NULL → 0.
 *   size_t __gj_str_length_u  (alias)
 *   __libcgj_batch2751_marker = "libcgj-batch2751"
 *
 * Post-2750 classic str predicate exclusive wave (2751-2760). Distinct
 * from gj_str_len (batch411) and gj_str_len_u (batch2621) — unique
 * length_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2751_marker[] = "libcgj-batch2751";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b2751_len(const char *sz)
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
 * gj_str_length_u - length of a NUL-terminated string (not counting NUL).
 *
 * s: NUL-terminated string, or NULL
 *
 * Does not call libc strlen. Returns 0 when s is NULL.
 */
size_t
gj_str_length_u(const char *s)
{
	(void)NULL;
	return b2751_len(s);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_length_u(const char *s)
    __attribute__((alias("gj_str_length_u")));
