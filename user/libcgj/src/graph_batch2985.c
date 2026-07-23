/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2985: JSON literal "null" prefix check (_u).
 *
 * Surface (unique symbols):
 *   int gj_json_is_null_lit_u(const char *s, size_t n);
 *     - Return 1 if s[0..3] is exactly the four octets "null" within
 *       bounds n. Else 0. Does not require a trailing delimiter
 *       (caller). NULL / short → 0.
 *   int __gj_json_is_null_lit_u  (alias)
 *   __libcgj_batch2985_marker = "libcgj-batch2985"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_json_match_null (batch1594 index form) — unique buffer-at-0
 * gj_json_is_null_lit_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2985_marker[] = "libcgj-batch2985";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if first 4 octets are "null". */
static int
b2985_is_null(const char *sz, size_t cN)
{
	if (cN < 4u) {
		return 0;
	}
	if (sz[0] != 'n' || sz[1] != 'u' || sz[2] != 'l' || sz[3] != 'l') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_null_lit_u - 1 if s begins with the four bytes "null".
 */
int
gj_json_is_null_lit_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL) {
		return 0;
	}
	return b2985_is_null(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_null_lit_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_is_null_lit_u")));
