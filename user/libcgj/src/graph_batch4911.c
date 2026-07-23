/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4911: freestanding UUID version-4 nibble stamp.
 *
 * Surface (unique symbols):
 *   int gj_uuid_set_version4_u(uint8_t b[16]);
 *     - In-place: set the 4-bit version field (high nibble of octet 6)
 *       to 4. Low nibble of octet 6 is preserved. Operate on the
 *       sequential u8[16] wire layout (not two u64 lo/hi halves).
 *       NULL -> no-op return 0; success -> 1.
 *   int __gj_uuid_set_version4_u  (alias)
 *   __libcgj_batch4911_marker = "libcgj-batch4911"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920:
 * set_version4_u, set_variant_rfc_u8, is_nil_u8, is_v4_u, cmp_u8,
 * copy_u8, swap_u, hash32_u, time_low_u, batch_id_4920). Distinct from
 * gj_uuid_set_version_u (batch2574, generic version) and
 * gj_uuid_set_v4_bits (batch628, version+variant together) — unique
 * gj_uuid_set_version4_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4911_marker[] = "libcgj-batch4911";

/* ---- freestanding helpers ---------------------------------------------- */

/* Stamp version high nibble = 4 on octet 6. pB must be non-NULL. */
static void
b4911_set_version4(uint8_t *pB)
{
	pB[6] = (uint8_t)((pB[6] & 0x0fu) | 0x40u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_set_version4_u - set UUID version nibble to 4 in place.
 *
 * b: 16-octet UUID wire value (NULL -> no-op, return 0)
 *
 * Preserves the low nibble of octet 6. Operates via u8[16] pointer
 * (preferred over lo/hi u64 halves). Returns 1 on success. Does not
 * call libc. No parent wires.
 */
int
gj_uuid_set_version4_u(uint8_t b[16])
{
	(void)NULL;
	if (b == NULL) {
		return 0;
	}
	b4911_set_version4(b);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_set_version4_u(uint8_t b[16])
    __attribute__((alias("gj_uuid_set_version4_u")));
