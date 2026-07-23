/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4912: freestanding RFC 4122 variant stamp (u8).
 *
 * Surface (unique symbols):
 *   int gj_uuid_set_variant_rfc_u8(uint8_t b[16]);
 *     - In-place: force RFC 4122 / 9562 variant bits 10xxxxxx on
 *       octet 8. Low 6 bits of octet 8 are preserved. NULL -> no-op
 *       return 0; success -> 1.
 *   int __gj_uuid_set_variant_rfc_u8  (alias)
 *   __libcgj_batch4912_marker = "libcgj-batch4912"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Distinct from gj_uuid_set_variant_rfc_u (batch2575, void, unsigned
 * char) — unique gj_uuid_set_variant_rfc_u8 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4912_marker[] = "libcgj-batch4912";

/* ---- freestanding helpers ---------------------------------------------- */

/* Force RFC 4122 variant 10xxxxxx on octet 8. pB must be non-NULL. */
static void
b4912_set_variant_rfc(uint8_t *pB)
{
	pB[8] = (uint8_t)((pB[8] & 0x3fu) | 0x80u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_set_variant_rfc_u8 - stamp RFC 4122 variant bits in place.
 *
 * b: 16-octet UUID wire value (NULL -> no-op, return 0)
 *
 * Wire layout (RFC 4122 / RFC 9562):
 *   octet 8 high 2 bits = 10 (variant); low 6 bits preserved
 *
 * Returns 1 on success. Does not call libc. No parent wires.
 */
int
gj_uuid_set_variant_rfc_u8(uint8_t b[16])
{
	(void)NULL;
	if (b == NULL) {
		return 0;
	}
	b4912_set_variant_rfc(b);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_set_variant_rfc_u8(uint8_t b[16])
    __attribute__((alias("gj_uuid_set_variant_rfc_u8")));
