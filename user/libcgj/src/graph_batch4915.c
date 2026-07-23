/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4915: freestanding UUID octet compare (-1/0/1).
 *
 * Surface (unique symbols):
 *   int gj_uuid_cmp_u8(const uint8_t a[16], const uint8_t b[16]);
 *     - Lexicographic unsigned-octet compare of two 16-byte UUIDs.
 *       Returns exactly -1, 0, or 1 (not arbitrary signed diffs).
 *       Both NULL -> 0; a NULL -> -1; b NULL -> 1; same pointer -> 0.
 *   int __gj_uuid_cmp_u8  (alias)
 *   __libcgj_batch4915_marker = "libcgj-batch4915"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_uuid_cmp_u (batch2576, returns raw octet diff) and
 * gj_uuid_cmp (batch468) — unique gj_uuid_cmp_u8 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4915_marker[] = "libcgj-batch4915";

/* ---- freestanding helpers ---------------------------------------------- */

/* Lexicographic unsigned-octet compare to -1/0/1. pA,pB non-NULL. */
static int
b4915_cmp(const uint8_t *pA, const uint8_t *pB)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (pA[i] < pB[i]) {
			return -1;
		}
		if (pA[i] > pB[i]) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_cmp_u8 - lexicographic unsigned-octet compare (-1/0/1).
 *
 * a, b: 16-octet UUID wire values (RFC 4122 sequential layout)
 *
 * Does not call libc memcmp. Early-exits on the first mismatch.
 * Return value is exactly -1, 0, or 1. No parent wires.
 */
int
gj_uuid_cmp_u8(const uint8_t a[16], const uint8_t b[16])
{
	(void)NULL;
	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	if (a == b) {
		return 0;
	}
	return b4915_cmp(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_cmp_u8(const uint8_t a[16], const uint8_t b[16])
    __attribute__((alias("gj_uuid_cmp_u8")));
