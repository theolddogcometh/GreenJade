/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4914: freestanding UUID version-4 predicate.
 *
 * Surface (unique symbols):
 *   int gj_uuid_is_v4_u(const uint8_t b[16]);
 *     - Return 1 if the version nibble (high nibble of octet 6) is 4.
 *       NULL -> 0. Does not require the RFC variant field.
 *   int __gj_uuid_is_v4_u  (alias)
 *   __libcgj_batch4914_marker = "libcgj-batch4914"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_uuid_version (batch163) and gj_uuid_set_v4_bits
 * (batch628) — unique gj_uuid_is_v4_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4914_marker[] = "libcgj-batch4914";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff version nibble is 4. pB must be non-NULL. */
static int
b4914_is_v4(const uint8_t *pB)
{
	return (((unsigned)pB[6] >> 4) & 0x0fu) == 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_is_v4_u - 1 iff the UUID version field is 4.
 *
 * b: 16-octet UUID wire value (NULL -> 0)
 *
 * Inspects only the high nibble of octet 6. Returns 1 when that nibble
 * is 4, else 0. Does not call libc. No parent wires.
 */
int
gj_uuid_is_v4_u(const uint8_t b[16])
{
	(void)NULL;
	if (b == NULL) {
		return 0;
	}
	return b4914_is_v4(b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_is_v4_u(const uint8_t b[16])
    __attribute__((alias("gj_uuid_is_v4_u")));
