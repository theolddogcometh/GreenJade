/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4381: UUID version nibble extract (_u,
 * uint8_t buffer).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_version_u2(const uint8_t b[16]);
 *     - High nibble of octet 6 (RFC 4122/9562 version field), 0..15.
 *       b == NULL returns 0.
 *   unsigned __gj_uuid_version_u2  (alias)
 *   __libcgj_batch4381_marker = "libcgj-batch4381"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390: uuid_version_u,
 * uuid_variant_u, bytes_eq16_u, bytes_eq32_u, bytes_is_zero16_u,
 * bytes_is_zero32_u, u128_xor_lo64_u, u128_xor_hi64_u, fnv1a32_u8_u,
 * batch_id_4390). UUID-ish / bytes unique wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4381_marker[] = "libcgj-batch4381";

/* ---- freestanding helpers ---------------------------------------------- */

/* High nibble of octet 6. pB must be non-NULL. */
static unsigned
b4381_version(const uint8_t *pB)
{
	return (unsigned)((pB[6] >> 4) & 0x0fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_version_u2 - high nibble of UUID octet 6 (version field).
 *
 * b: 16-octet UUID (NULL -> 0)
 *
 * Returns 0..15 on success, 0 if b is NULL. Does not call libc.
 * Self-contained; no parent wires.
 */
unsigned
gj_uuid_version_u2(const uint8_t pB[16])
{
	(void)NULL;
	if (pB == NULL) {
		return 0u;
	}
	return b4381_version(pB);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_version_u2(const uint8_t pB[16])
    __attribute__((alias("gj_uuid_version_u2")));
