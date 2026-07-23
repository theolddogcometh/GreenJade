/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1417: URI query-marker predicate.
 *
 * Surface (unique symbols):
 *   int gj_uri_has_query(const char *uri);
 *     — 1 if uri contains a '?' (query delimiter) before any '#',
 *       else 0. Fragment-only '?' after '#' is ignored (not counted).
 *       NULL → 0.
 *   int __gj_uri_has_query  (alias)
 *   __libcgj_batch1417_marker = "libcgj-batch1417"
 *
 * Distinct from gj_uri_has_fragment / full URI parse — unique name.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1417_marker[] = "libcgj-batch1417";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_has_query — 1 if a query marker '?' appears before '#'.
 *
 * Examples:
 *   "http://x?y=1"   → 1
 *   "http://x#a?b"   → 0  ('?' is inside fragment)
 *   "http://x"       → 0
 *   "?only"          → 1
 *   NULL             → 0
 */
int
gj_uri_has_query(const char *szUri)
{
	size_t i;

	if (szUri == NULL) {
		return 0;
	}

	for (i = 0u; szUri[i] != '\0'; i++) {
		if (szUri[i] == '#') {
			return 0;
		}
		if (szUri[i] == '?') {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uri_has_query(const char *szUri)
    __attribute__((alias("gj_uri_has_query")));
