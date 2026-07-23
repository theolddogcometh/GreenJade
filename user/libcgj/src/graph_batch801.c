/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch801: freestanding whitespace-presence predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_has_ws(const char *s);
 *     — Return 1 if s contains at least one space/tab/CR/LF octet,
 *       else 0. s == NULL or empty → 0.
 *   int __gj_str_has_ws  (alias)
 *   __libcgj_batch801_marker = "libcgj-batch801"
 *
 * Distinct from gj_str_is_blank (batch761, all-blank) — unique
 * gj_str_has_ws surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch801_marker[] = "libcgj-batch801";

/* ---- helpers ----------------------------------------------------------- */

/* True if ch is ASCII whitespace: space, HT, LF, or CR. */
static int
b801_is_ws(unsigned char uCh)
{
	return (uCh == (unsigned char)' '
	    || uCh == (unsigned char)'\t'
	    || uCh == (unsigned char)'\n'
	    || uCh == (unsigned char)'\r') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_ws — 1 if s contains any space/tab/CR/LF octet.
 *
 * s: NUL-terminated C string (NULL → 0)
 *
 * Does not treat vertical tab / form feed as whitespace. ASCII only.
 */
int
gj_str_has_ws(const char *sz)
{
	size_t i;

	if (sz == NULL) {
		return 0;
	}
	for (i = 0u; sz[i] != '\0'; i++) {
		if (b801_is_ws((unsigned char)sz[i])) {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_ws(const char *sz)
    __attribute__((alias("gj_str_has_ws")));
