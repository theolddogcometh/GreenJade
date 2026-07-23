/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2573: freestanding UUID variant class (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_variant_u(const unsigned char u[16]);
 *     - Return the variant class of octet 8 (libuuid-compatible codes):
 *         0 = NCS, 1 = DCE / RFC 4122, 2 = Microsoft, 3 = other.
 *       u == NULL returns 0.
 *   unsigned __gj_uuid_variant_u  (alias)
 *   __libcgj_batch2573_marker = "libcgj-batch2573"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_variant
 * (batch163, NULL -> -1), gj_uuid_var_u (batch746, raw top-2 bits), and
 * gj_uuid_variant_ok (batch2098) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2573_marker[] = "libcgj-batch2573";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Classify variant bits of octet 8:
 *   0  NCS compatibility      (0xxx xxxx)
 *   1  DCE / RFC 4122 / 9562  (10xx xxxx)
 *   2  Microsoft              (110x xxxx)
 *   3  reserved / other       (111x xxxx)
 * pU must be non-NULL.
 */
static unsigned
b2573_variant(const unsigned char *pU)
{
	unsigned char u8C;

	u8C = pU[8];
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
 * gj_uuid_variant_u - variant class of UUID octet 8.
 *
 * u: 16-octet UUID (NULL -> 0)
 *
 * Returns 0..3 on success, 0 if u is NULL. Does not call libc.
 */
unsigned
gj_uuid_variant_u(const unsigned char pU[16])
{
	(void)NULL;
	if (pU == NULL) {
		return 0u;
	}
	return b2573_variant(pU);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_variant_u(const unsigned char pU[16])
    __attribute__((alias("gj_uuid_variant_u")));
