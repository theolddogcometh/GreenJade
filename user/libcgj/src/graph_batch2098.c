/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2098: freestanding RFC 4122 UUID variant check.
 *
 * Surface (unique symbols):
 *   int gj_uuid_variant_ok(const unsigned char uuid[16]);
 *     - Return 1 if octet 8 has the RFC 4122 / DCE 1.1 variant layout
 *       (bits 10xx xxxx), else 0. uuid == NULL returns 0.
 *   int __gj_uuid_variant_ok  (alias)
 *   __libcgj_batch2098_marker = "libcgj-batch2098"
 *
 * Post-2000 id exclusive wave (2091-2100). Distinct from
 * gj_uuid_variant (batch163) which returns a class code - this is a
 * boolean ok predicate; unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2098_marker[] = "libcgj-batch2098";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if high two bits of octet 8 are 10 (RFC 4122). */
static int
b2098_variant_ok(const unsigned char *pU)
{
	unsigned char u8C;

	u8C = pU[8];
	return ((u8C & 0xc0u) == 0x80u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_variant_ok - true if UUID uses the RFC 4122 variant bits.
 *
 * uuid: 16-octet UUID (NULL -> 0)
 *
 * Returns 1 when (uuid[8] & 0xC0) == 0x80, else 0. Does not call libc.
 */
int
gj_uuid_variant_ok(const unsigned char pUuid[16])
{
	(void)NULL;
	if (pUuid == NULL) {
		return 0;
	}
	return b2098_variant_ok(pUuid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_uuid_variant_ok(const unsigned char pUuid[16])
    __attribute__((alias("gj_uuid_variant_ok")));
