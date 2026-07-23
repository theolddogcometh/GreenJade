/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2575: freestanding RFC 4122 variant setter (_u).
 *
 * Surface (unique symbols):
 *   void gj_uuid_set_variant_rfc_u(unsigned char u[16]);
 *     - In-place: force RFC 4122 / 9562 variant bits 10xxxxxx on
 *       octet 8. Low 6 bits of octet 8 are preserved. NULL -> no-op.
 *   void __gj_uuid_set_variant_rfc_u  (alias)
 *   __libcgj_batch2575_marker = "libcgj-batch2575"
 *
 * UUID exclusive wave (2571-2580). Distinct from gj_uuid_set_v4_bits
 * (batch628, which also stamps version 4) — variant-only stamp;
 * unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2575_marker[] = "libcgj-batch2575";

/* ---- freestanding helpers ---------------------------------------------- */

/* Force RFC 4122 variant 10xxxxxx on octet 8. pU must be non-NULL. */
static void
b2575_set_variant_rfc(unsigned char *pU)
{
	pU[8] = (unsigned char)((pU[8] & 0x3fu) | 0x80u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_set_variant_rfc_u - stamp RFC 4122 variant bits in place.
 *
 * u: 16-octet UUID (NULL -> no-op)
 *
 * Wire layout (RFC 4122 / RFC 9562):
 *   octet 8 high 2 bits = 10 (variant); low 6 bits preserved
 *
 * Does not call libc.
 */
void
gj_uuid_set_variant_rfc_u(unsigned char pU[16])
{
	(void)NULL;
	if (pU == NULL) {
		return;
	}
	b2575_set_variant_rfc(pU);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_uuid_set_variant_rfc_u(unsigned char pU[16])
    __attribute__((alias("gj_uuid_set_variant_rfc_u")));
