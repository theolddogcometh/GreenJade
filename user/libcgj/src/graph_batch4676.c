/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4676: all-whitespace / empty C-string
 * predicate (string-measure wave).
 *
 * Surface (unique symbols):
 *   int gj_str_is_ws_u(const char *s);
 *     - Return 1 if every octet of s is space / tab / CR / LF, or s is
 *       the empty string. NULL → 0. Other octets → 0. ASCII whitespace
 *       only (no locale / no VT / no form feed).
 *   int __gj_str_is_ws_u  (alias)
 *   __libcgj_batch4676_marker = "libcgj-batch4676"
 *
 * Exclusive continuum CREATE-ONLY (4671-4680). Distinct from
 * gj_str_is_ws_only_u (batch3878) — unique gj_str_is_ws_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4676_marker[] = "libcgj-batch4676";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is space, tab, CR, or LF. */
static int
b4676_is_ws(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

static int
b4676_is_ws_str(const char *sz)
{
	size_t i;

	i = 0u;
	while (sz[i] != '\0') {
		if (!b4676_is_ws((unsigned char)sz[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_ws_u - 1 if s is empty or only space/tab/CR/LF.
 *
 * s == NULL → 0. Empty string → 1. Does not call libc isspace.
 * No parent wires.
 */
int
gj_str_is_ws_u(const char *s)
{
	(void)NULL;
	if (s == NULL) {
		return 0;
	}
	return b4676_is_ws_str(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_ws_u(const char *s)
    __attribute__((alias("gj_str_is_ws_u")));
