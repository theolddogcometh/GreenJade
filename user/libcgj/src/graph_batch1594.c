/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1594: match JSON literal "null".
 *
 * Surface (unique symbols):
 *   int gj_json_match_null(const char *s, size_t n, size_t i);
 *     — Return 1 if s[i..i+3] is exactly "null" within bounds n.
 *       Else 0. Does not require a trailing delimiter check (caller).
 *   int __gj_json_match_null  (alias)
 *   __libcgj_batch1594_marker = "libcgj-batch1594"
 *
 * Distinct from gj_json_get_bool / other value extractors — unique name.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1594_marker[] = "libcgj-batch1594";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1594_match4(const char *sz, size_t cN, size_t cI,
    char ch0, char ch1, char ch2, char ch3)
{
	if (sz == NULL) {
		return 0;
	}
	if (cI >= cN || (cN - cI) < 4u) {
		return 0;
	}
	if (sz[cI] != ch0 || sz[cI + 1u] != ch1 || sz[cI + 2u] != ch2 ||
	    sz[cI + 3u] != ch3) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_match_null — 1 if s[i..] begins with the four bytes "null".
 */
int
gj_json_match_null(const char *sz, size_t cN, size_t cI)
{
	return b1594_match4(sz, cN, cI, 'n', 'u', 'l', 'l');
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_match_null(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_json_match_null")));
