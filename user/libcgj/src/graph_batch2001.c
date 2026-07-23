/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2001: unsigned empty C-string predicate.
 *
 * Surface (unique symbols):
 *   unsigned gj_str_is_empty_u(const char *s);
 *     — Return 1u if s is NULL or s[0] == '\0', else 0u. Unsigned
 *       boolean twin of gj_str_is_empty (batch731 int surface).
 *   unsigned __gj_str_is_empty_u  (alias)
 *   __libcgj_batch2001_marker = "libcgj-batch2001"
 *
 * Post-2000 continuum str helpers exclusive wave (2001–2010). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2001_marker[] = "libcgj-batch2001";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b2001_empty_u(const char *sz)
{
	if (sz == NULL) {
		return 1u;
	}
	if (sz[0] == '\0') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_empty_u — 1u if s is NULL or the empty string, else 0u.
 *
 * s: NUL-terminated C string, or NULL
 *
 * Does not walk past the first character.
 */
unsigned
gj_str_is_empty_u(const char *s)
{
	(void)NULL;
	return b2001_empty_u(s);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_str_is_empty_u(const char *s)
    __attribute__((alias("gj_str_is_empty_u")));
