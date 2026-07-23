/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2002: nonempty C-string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_nonempty_p(const char *s);
 *     — Return 1 if s is non-NULL and s[0] != '\0', else 0. Inverse
 *       of the empty-string predicates (gj_str_is_empty / _u).
 *   int __gj_str_nonempty_p  (alias)
 *   __libcgj_batch2002_marker = "libcgj-batch2002"
 *
 * Post-2000 continuum str helpers exclusive wave (2001–2010). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2002_marker[] = "libcgj-batch2002";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2002_nonempty_p(const char *sz)
{
	if (sz == NULL) {
		return 0;
	}
	if (sz[0] == '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_nonempty_p — 1 if s is a non-NULL nonempty C string, else 0.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Does not walk past the first character.
 */
int
gj_str_nonempty_p(const char *s)
{
	(void)NULL;
	return b2002_nonempty_p(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_nonempty_p(const char *s)
    __attribute__((alias("gj_str_nonempty_p")));
