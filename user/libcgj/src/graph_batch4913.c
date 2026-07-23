/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4913: freestanding UUID nil predicate (u8).
 *
 * Surface (unique symbols):
 *   int gj_uuid_is_nil_u8(const uint8_t b[16]);
 *     - Return 1 if all 16 octets are zero (nil UUID). Return 0 for
 *       non-nil or NULL.
 *   int __gj_uuid_is_nil_u8  (alias)
 *   __libcgj_batch4913_marker = "libcgj-batch4913"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_uuid_is_nil_u (batch2571), gj_uuid_is_nil
 * (batch467), and gj_uuid_nil_p (batch629) — unique gj_uuid_is_nil_u8
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4913_marker[] = "libcgj-batch4913";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff all 16 octets are zero. pB must be non-NULL. */
static int
b4913_is_nil(const uint8_t *pB)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (pB[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_is_nil_u8 - 1 iff b[0..15] are all zero.
 *
 * b: 16-octet UUID (NULL -> 0)
 *
 * Returns 1 for the nil UUID, else 0. Does not call libc. No parent
 * wires.
 */
int
gj_uuid_is_nil_u8(const uint8_t b[16])
{
	(void)NULL;
	if (b == NULL) {
		return 0;
	}
	return b4913_is_nil(b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_is_nil_u8(const uint8_t b[16])
    __attribute__((alias("gj_uuid_is_nil_u8")));
