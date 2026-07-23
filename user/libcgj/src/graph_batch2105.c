/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2105: span whitespace-presence predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_has_ws_p(const char *s, size_t n);
 *     - Return 1 if s[0..n) contains at least one ASCII whitespace octet
 *       (space, HT, LF, VT, FF, CR: 0x09..0x0D, 0x20), else 0.
 *       s == NULL or n == 0 -> 0. Does not require a terminating NUL.
 *   int __gj_str_has_ws_p  (alias)
 *   __libcgj_batch2105_marker = "libcgj-batch2105"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Distinct from
 * gj_str_has_ws (batch801, NUL-terminated, space/tab/CR/LF only) -
 * unique _p span surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2105_marker[] = "libcgj-batch2105";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2105_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

static int
b2105_has_ws(const char *sz, size_t cb)
{
	size_t i;

	if (sz == NULL || cb == 0u) {
		return 0;
	}
	for (i = 0u; i < cb; i++) {
		if (b2105_is_ws((unsigned char)sz[i])) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_ws_p - 1 if span s[0..n) contains any ASCII whitespace.
 *
 * s: buffer of n octets (NULL -> 0)
 * n: span length
 *
 * Early-exits on the first match. Does not call libc.
 */
int
gj_str_has_ws_p(const char *s, size_t n)
{
	return b2105_has_ws(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_ws_p(const char *s, size_t n)
    __attribute__((alias("gj_str_has_ws_p")));
