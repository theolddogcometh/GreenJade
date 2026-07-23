/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1418: URI fragment-marker predicate.
 *
 * Surface (unique symbols):
 *   int gj_uri_has_fragment(const char *uri);
 *     — 1 if uri contains a '#' (fragment delimiter), else 0.
 *       NULL → 0.
 *   int __gj_uri_has_fragment  (alias)
 *   __libcgj_batch1418_marker = "libcgj-batch1418"
 *
 * Distinct from gj_uri_has_query / full URI parse — unique name.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1418_marker[] = "libcgj-batch1418";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uri_has_fragment — 1 if a '#' fragment marker is present.
 *
 * Examples:
 *   "http://x#frag"  → 1
 *   "http://x?y=1"   → 0
 *   "#"              → 1
 *   NULL             → 0
 */
int
gj_uri_has_fragment(const char *szUri)
{
	size_t i;

	if (szUri == NULL) {
		return 0;
	}

	for (i = 0u; szUri[i] != '\0'; i++) {
		if (szUri[i] == '#') {
			return 1;
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uri_has_fragment(const char *szUri)
    __attribute__((alias("gj_uri_has_fragment")));
