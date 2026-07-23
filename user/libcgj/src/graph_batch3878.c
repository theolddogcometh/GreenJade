/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3878: whitespace-only string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_ws_only_u(const char *s);
 *     - Return 1 if every octet of s is space / tab / CR / LF
 *       (vacuously true for the empty string). NULL → 0. Other octets
 *       → 0. ASCII whitespace only; no locale / no vertical tab / form feed.
 *   int __gj_str_is_ws_only_u  (alias)
 *   __libcgj_batch3878_marker = "libcgj-batch3878"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880). Unique
 * gj_str_is_ws_only_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3878_marker[] = "libcgj-batch3878";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if ch is space, tab, CR, or LF. */
static int
b3878_is_ws(unsigned char u8Ch)
{
	return u8Ch == (unsigned char)' ' ||
	    u8Ch == (unsigned char)'\t' ||
	    u8Ch == (unsigned char)'\r' ||
	    u8Ch == (unsigned char)'\n';
}

static int
b3878_ws_only(const char *sz)
{
	size_t i;

	i = 0u;
	while (sz[i] != '\0') {
		if (!b3878_is_ws((unsigned char)sz[i])) {
			return 0;
		}
		i++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_ws_only_u - 1 if s is empty or only space/tab/CR/LF.
 *
 * s == NULL → 0. Empty string → 1. Does not call libc isspace.
 * No parent wires.
 */
int
gj_str_is_ws_only_u(const char *s)
{
	(void)NULL;
	if (s == NULL) {
		return 0;
	}
	return b3878_ws_only(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_ws_only_u(const char *s)
    __attribute__((alias("gj_str_is_ws_only_u")));
