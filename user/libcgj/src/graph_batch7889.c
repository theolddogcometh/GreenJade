/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7889: strcoll_l-style stub (locale ignored).
 *
 * Surface (unique symbols):
 *   int gj_strcoll_l_stub_7889(const char *a, const char *b,
 *                              uint32_t loc_tag);
 *     - C-locale collate of a and b; loc_tag is ignored (soft locale
 *       handle stand-in). NULL handling matches gj_strcoll_c_7881.
 *   int __gj_strcoll_l_stub_7889  (alias)
 *   __libcgj_batch7889_marker = "libcgj-batch7889"
 *
 * Exclusive continuum CREATE-ONLY (7881-7890: strcoll/strxfrm stubs).
 * Unique gj_strcoll_l_stub_7889 surface only; no multi-def with libc
 * strcoll_l. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7889_marker[] = "libcgj-batch7889";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b7889_coll(const unsigned char *pA, const unsigned char *pB)
{
	size_t i;

	i = 0u;
	while (pA[i] != 0u && pA[i] == pB[i]) {
		i++;
	}
	return (int)pA[i] - (int)pB[i];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcoll_l_stub_7889 - strcoll_l-shaped C-locale stub.
 *
 * a, b:    NUL strings
 * loc_tag: ignored soft locale tag (any uint32_t)
 *
 * Soft freestanding only; does not resolve locales. No parent wires.
 */
int
gj_strcoll_l_stub_7889(const char *a, const char *b, uint32_t loc_tag)
{
	(void)NULL;
	(void)loc_tag;
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	return b7889_coll((const unsigned char *)a,
	    (const unsigned char *)b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_strcoll_l_stub_7889(const char *a, const char *b, uint32_t loc_tag)
    __attribute__((alias("gj_strcoll_l_stub_7889")));
