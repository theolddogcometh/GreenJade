/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch761: freestanding blank-string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_blank(const char *s);
 *     — Return 1 if s is NULL or every octet is space/tab/CR/LF,
 *       else 0. Empty string is blank (returns 1).
 *   int __gj_str_is_blank  (alias)
 *   __libcgj_batch761_marker = "libcgj-batch761"
 *
 * Unique gj_str_is_blank surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch761_marker[] = "libcgj-batch761";

/* ---- helpers ----------------------------------------------------------- */

/* True if ch is ASCII blank: space, HT, LF, or CR. */
static int
b761_is_blank_ch(unsigned char uCh)
{
	return (uCh == (unsigned char)' '
	    || uCh == (unsigned char)'\t'
	    || uCh == (unsigned char)'\n'
	    || uCh == (unsigned char)'\r') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_blank — 1 if s is NULL or only space/tab/CR/LF octets.
 *
 * s: NUL-terminated C string (NULL → 1)
 *
 * Does not treat vertical tab / form feed as blank. ASCII only.
 */
int
gj_str_is_blank(const char *sz)
{
	size_t i;

	if (sz == NULL) {
		return 1;
	}
	for (i = 0u; sz[i] != '\0'; i++) {
		if (!b761_is_blank_ch((unsigned char)sz[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_blank(const char *sz)
    __attribute__((alias("gj_str_is_blank")));
