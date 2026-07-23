/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2576: freestanding UUID octet-array compare (_u).
 *
 * Surface (unique symbols):
 *   int gj_uuid_cmp_u(const unsigned char a[16], const unsigned char b[16]);
 *     - Lexicographic unsigned-octet compare of two 16-byte UUIDs.
 *       Returns <0 if a < b, 0 if equal, >0 if a > b
 *       (first differing octet: (int)a[i] - (int)b[i]).
 *       Both NULL -> 0; a NULL -> -1; b NULL -> 1; same pointer -> 0.
 *   int __gj_uuid_cmp_u  (alias)
 *   __libcgj_batch2576_marker = "libcgj-batch2576"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_cmp (batch468)
 * and uuid_compare (batch35) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2576_marker[] = "libcgj-batch2576";

/* ---- freestanding helpers ---------------------------------------------- */

/* Lexicographic unsigned-octet compare. pA and pB must be non-NULL. */
static int
b2576_cmp(const unsigned char *pA, const unsigned char *pB)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (pA[i] != pB[i]) {
			return (int)pA[i] - (int)pB[i];
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_cmp_u - lexicographic unsigned-octet compare of two UUIDs.
 *
 * a, b: 16-octet UUID wire values (RFC 4122 sequential layout)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 * Return value is negative / zero / positive (<0 0 >0).
 */
int
gj_uuid_cmp_u(const unsigned char pA[16], const unsigned char pB[16])
{
	(void)NULL;
	if (pA == NULL || pB == NULL) {
		if (pA == pB) {
			return 0;
		}
		return (pA == NULL) ? -1 : 1;
	}
	if (pA == pB) {
		return 0;
	}
	return b2576_cmp(pA, pB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_cmp_u(const unsigned char pA[16], const unsigned char pB[16])
    __attribute__((alias("gj_uuid_cmp_u")));
