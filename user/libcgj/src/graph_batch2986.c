/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2986: JSON literal "true" prefix check (_u).
 *
 * Surface (unique symbols):
 *   int gj_json_is_true_lit_u(const char *s, size_t n);
 *     - Return 1 if s[0..3] is exactly the four octets "true" within
 *       bounds n. Else 0. Does not require a trailing delimiter
 *       (caller). NULL / short → 0.
 *   int __gj_json_is_true_lit_u  (alias)
 *   __libcgj_batch2986_marker = "libcgj-batch2986"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990). Distinct from
 * gj_json_match_true (batch1595 index form) — unique buffer-at-0
 * gj_json_is_true_lit_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2986_marker[] = "libcgj-batch2986";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if first 4 octets are "true". */
static int
b2986_is_true(const char *sz, size_t cN)
{
	if (cN < 4u) {
		return 0;
	}
	if (sz[0] != 't' || sz[1] != 'r' || sz[2] != 'u' || sz[3] != 'e') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_true_lit_u - 1 if s begins with the four bytes "true".
 */
int
gj_json_is_true_lit_u(const char *sz, size_t cN)
{
	(void)NULL;
	if (sz == NULL) {
		return 0;
	}
	return b2986_is_true(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_true_lit_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_is_true_lit_u")));
