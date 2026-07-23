/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1596: match JSON literal "false".
 *
 * Surface (unique symbols):
 *   int gj_json_match_false(const char *s, size_t n, size_t i);
 *     — Return 1 if s[i..i+4] is exactly "false" within bounds n.
 *       Else 0. Does not require a trailing delimiter check (caller).
 *   int __gj_json_match_false  (alias)
 *   __libcgj_batch1596_marker = "libcgj-batch1596"
 *
 * Distinct from gj_json_get_bool (batch107) — unique name.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1596_marker[] = "libcgj-batch1596";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1596_match5(const char *sz, size_t cN, size_t cI)
{
	if (sz == NULL) {
		return 0;
	}
	if (cI >= cN || (cN - cI) < 5u) {
		return 0;
	}
	if (sz[cI] != 'f' || sz[cI + 1u] != 'a' || sz[cI + 2u] != 'l' ||
	    sz[cI + 3u] != 's' || sz[cI + 4u] != 'e') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_match_false — 1 if s[i..] begins with the five bytes "false".
 */
int
gj_json_match_false(const char *sz, size_t cN, size_t cI)
{
	return b1596_match5(sz, cN, cI);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_match_false(const char *sz, size_t cN, size_t cI)
    __attribute__((alias("gj_json_match_false")));
