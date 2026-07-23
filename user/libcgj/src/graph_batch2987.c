/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2987: JSON literal "false" prefix check (_u).
 *
 * Surface (unique symbols):
 *   int gj_json_is_false_lit_u(const char *s, size_t n);
 *     - Return 1 if s[0..4] is exactly the five octets "false" within
 *       bounds n. Else 0. Does not require a trailing delimiter
 *       (caller). NULL / short → 0.
 *   int __gj_json_is_false_lit_u  (alias)
 *   __libcgj_batch2987_marker = "libcgj-batch2987"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_json_match_false (batch1596 index form) — unique buffer-at-0
 * gj_json_is_false_lit_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2987_marker[] = "libcgj-batch2987";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if first 5 octets are "false". */
static int
b2987_is_false(const char *sz, size_t cN)
{
	if (cN < 5u) {
		return 0;
	}
	if (sz[0] != 'f' || sz[1] != 'a' || sz[2] != 'l' ||
	    sz[3] != 's' || sz[4] != 'e') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_false_lit_u - 1 if s begins with the five bytes "false".
 */
int
gj_json_is_false_lit_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL) {
		return 0;
	}
	return b2987_is_false(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_false_lit_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_is_false_lit_u")));
