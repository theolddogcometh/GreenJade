/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch468: freestanding UUID octet-array compare.
 *
 * Surface (unique symbols only — does NOT redefine uuid_compare / uuid_equal
 * from graph_batch35.c or gj_mem_cmp from graph_batch399.c):
 *   int gj_uuid_cmp(const uint8_t a[16], const uint8_t b[16]);
 *     — Lexicographic unsigned-octet compare of two 16-byte UUID values.
 *       Returns <0 if a < b, 0 if equal, >0 if a > b
 *       (first differing octet: (int)a[i] - (int)b[i]).
 *       Both NULL → 0; a NULL → -1; b NULL → 1; same pointer → 0.
 *   int __gj_uuid_cmp  (alias)
 *   __libcgj_batch468_marker = "libcgj-batch468"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   uuid_compare / uuid_equal             → graph_batch35.c
 *   gj_uuid_parse / gj_uuid_unparse       → graph_batch116.c
 *   gj_uuid_parse_strict                  → graph_batch332.c
 *   gj_mem_cmp                            → graph_batch399.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party UUID source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch468_marker[] = "libcgj-batch468";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_cmp — lexicographic unsigned-octet compare of two 16-byte UUIDs.
 *
 * a, b: 16-octet UUID wire values (RFC 4122 sequential layout)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 * Return value is negative / zero / positive (<0 0 >0).
 */
int
gj_uuid_cmp(const uint8_t a[16], const uint8_t b[16])
{
	unsigned i;

	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	if (a == b) {
		return 0;
	}

	for (i = 0u; i < 16u; i++) {
		if (a[i] != b[i]) {
			return (int)a[i] - (int)b[i];
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_cmp(const uint8_t a[16], const uint8_t b[16])
    __attribute__((alias("gj_uuid_cmp")));
