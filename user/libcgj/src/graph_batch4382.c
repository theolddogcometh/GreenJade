/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4382: UUID variant class extract (_u,
 * uint8_t buffer).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_variant_u2(const uint8_t b[16]);
 *     - Variant class of octet 8 (libuuid-compatible codes):
 *         0 = NCS, 1 = DCE / RFC 4122, 2 = Microsoft, 3 = other.
 *       b == NULL returns 0.
 *   unsigned __gj_uuid_variant_u2  (alias)
 *   __libcgj_batch4382_marker = "libcgj-batch4382"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390). UUID-ish / bytes unique
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4382_marker[] = "libcgj-batch4382";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Classify variant bits of octet 8:
 *   0  NCS compatibility      (0xxx xxxx)
 *   1  DCE / RFC 4122 / 9562  (10xx xxxx)
 *   2  Microsoft              (110x xxxx)
 *   3  reserved / other       (111x xxxx)
 * pB must be non-NULL.
 */
static unsigned
b4382_variant(const uint8_t *pB)
{
	uint8_t u8C;

	u8C = pB[8];
	if ((u8C & 0x80u) == 0u) {
		return 0u; /* NCS */
	}
	if ((u8C & 0xc0u) == 0x80u) {
		return 1u; /* DCE / RFC 4122 */
	}
	if ((u8C & 0xe0u) == 0xc0u) {
		return 2u; /* Microsoft */
	}
	return 3u; /* other */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_variant_u2 - variant class of UUID octet 8.
 *
 * b: 16-octet UUID (NULL -> 0)
 *
 * Returns 0..3 on success, 0 if b is NULL. Does not call libc.
 * Self-contained; no parent wires.
 */
unsigned
gj_uuid_variant_u2(const uint8_t pB[16])
{
	(void)NULL;
	if (pB == NULL) {
		return 0u;
	}
	return b4382_variant(pB);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_variant_u2(const uint8_t pB[16])
    __attribute__((alias("gj_uuid_variant_u2")));
